#include "Implementation.h"
#include <virtualinput/virtualinput.h>

int g_pipefd[2];
struct Message {
    uint32_t code;
    actiontype type;
};

static const char * connectorName = "/tmp/keyhandler";
static void VirtualKeyboardCallback(actiontype type , unsigned int code) {
    if (type != COMPLETED) {
        Message message;
        message.code = code;
        message.type = type;
        write(g_pipefd[1], &message, sizeof(message));
    }
}

namespace WPEFramework {
namespace Rpi {

Display::SurfaceImplementation::SurfaceImplementation(
        Display& display, const std::string& name,
        const uint32_t width, const uint32_t height)
: _parent(display)
, _refcount(1)
, _name(name)
, _x(0)
, _y(0)
, _width(width)
, _height(height)
, _nativeWindow(nullptr)
, _keyboard(nullptr) {

    VC_RECT_T dst_rect;
    vc_dispmanx_rect_set(&dst_rect, 0, 0, _width, _height);
    VC_RECT_T src_rect;
    vc_dispmanx_rect_set(&src_rect, 0, 0, (_width << 16), (_height << 16));

    dispman_display = vc_dispmanx_display_open(0);
    dispman_update = vc_dispmanx_update_start(0);
    dispman_element = vc_dispmanx_element_add(
            dispman_update,
            dispman_display,
            2 /*layer*/,
            &dst_rect,
            0 /*src*/,
            &src_rect,
            DISPMANX_PROTECTION_NONE,
            0 /*alpha*/,
            0 /*clamp*/,
            DISPMANX_NO_ROTATE);
    vc_dispmanx_update_submit_sync(dispman_update);

    nativeWindow.element = dispman_element;
    nativeWindow.width = _width;
    nativeWindow.height = _height;
    _nativeWindow = static_cast<EGLSurface>(&nativeWindow);

    _parent.Register(this);
}

Display::SurfaceImplementation::~SurfaceImplementation() {

    dispman_update = vc_dispmanx_update_start(0);
    vc_dispmanx_element_remove(dispman_update, dispman_element);
    vc_dispmanx_update_submit_sync(dispman_update);
    vc_dispmanx_display_close(dispman_display);

    _parent.Unregister(this);
}

Display::Display(const std::string& name)
: _displayName(name)
, _virtualkeyboard(nullptr)  {

    if (pipe(g_pipefd) == -1) {
        g_pipefd[0] = -1;
        g_pipefd[1] = -1;
    }

    _virtualkeyboard = Construct(
            name.c_str(), connectorName, VirtualKeyboardCallback);
    if (_virtualkeyboard == nullptr) {
        fprintf(stderr, "Initialization of virtual keyboard failed!!!\n");
    }
}

Display::~Display() {

    if (_virtualkeyboard != nullptr) {
        Destruct(_virtualkeyboard);
    }
}

int Display::Process(const uint32_t data) {
    Message message;
    if ((data != 0) && (g_pipefd[0] != -1) &&
            (read(g_pipefd[0], &message, sizeof(message)) > 0)) {

        std::list<SurfaceImplementation*>::iterator index(_surfaces.begin());
        while (index != _surfaces.end()) {
            (*index)->SendKey(
                    message.code, (message.type == 0 ?
                            IDisplay::IKeyboard::released :
                            IDisplay::IKeyboard::pressed), time(nullptr));
            index++;
        }
    }
    return (0);
}

int Display::FileDescriptor() const {
    return (g_pipefd[0]);
}

Compositor::IDisplay::ISurface* Display::Create(
        const std::string& name, const uint32_t width, const uint32_t height) {
    return (new SurfaceImplementation(*this, name, width, height));
}

Compositor::IDisplay* Display::Instance(const std::string& displayName) {
    static Display myDisplay(displayName);
    return (&myDisplay);
}
}

Compositor::IDisplay* Compositor::IDisplay::Instance(
        const std::string& displayName) {
    bcm_host_init();
    return (Rpi::Display::Instance(displayName));
}
}
