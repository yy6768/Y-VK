#include "Window.h"
#include "GLFW/glfw3.h"
#include "App/Application.h"
#include "Gui/InputEvent.h"

KeyCode GlfwTranslateKeyCode(int key) {
    static const std::unordered_map<int, KeyCode> key_lookup =
        {
            {GLFW_KEY_SPACE, KeyCode::Space},
            {GLFW_KEY_APOSTROPHE, KeyCode::Apostrophe},
            {GLFW_KEY_COMMA, KeyCode::Comma},
            {GLFW_KEY_MINUS, KeyCode::Minus},
            {GLFW_KEY_PERIOD, KeyCode::Period},
            {GLFW_KEY_SLASH, KeyCode::Slash},
            {GLFW_KEY_0, KeyCode::_0},
            {GLFW_KEY_1, KeyCode::_1},
            {GLFW_KEY_2, KeyCode::_2},
            {GLFW_KEY_3, KeyCode::_3},
            {GLFW_KEY_4, KeyCode::_4},
            {GLFW_KEY_5, KeyCode::_5},
            {GLFW_KEY_6, KeyCode::_6},
            {GLFW_KEY_7, KeyCode::_7},
            {GLFW_KEY_8, KeyCode::_8},
            {GLFW_KEY_9, KeyCode::_9},
            {GLFW_KEY_SEMICOLON, KeyCode::Semicolon},
            {GLFW_KEY_EQUAL, KeyCode::Equal},
            {GLFW_KEY_A, KeyCode::A},
            {GLFW_KEY_B, KeyCode::B},
            {GLFW_KEY_C, KeyCode::C},
            {GLFW_KEY_D, KeyCode::D},
            {GLFW_KEY_E, KeyCode::E},
            {GLFW_KEY_F, KeyCode::F},
            {GLFW_KEY_G, KeyCode::G},
            {GLFW_KEY_H, KeyCode::H},
            {GLFW_KEY_I, KeyCode::I},
            {GLFW_KEY_J, KeyCode::J},
            {GLFW_KEY_K, KeyCode::K},
            {GLFW_KEY_L, KeyCode::L},
            {GLFW_KEY_M, KeyCode::M},
            {GLFW_KEY_N, KeyCode::N},
            {GLFW_KEY_O, KeyCode::O},
            {GLFW_KEY_P, KeyCode::P},
            {GLFW_KEY_Q, KeyCode::Q},
            {GLFW_KEY_R, KeyCode::R},
            {GLFW_KEY_S, KeyCode::S},
            {GLFW_KEY_T, KeyCode::T},
            {GLFW_KEY_U, KeyCode::U},
            {GLFW_KEY_V, KeyCode::V},
            {GLFW_KEY_W, KeyCode::W},
            {GLFW_KEY_X, KeyCode::X},
            {GLFW_KEY_Y, KeyCode::Y},
            {GLFW_KEY_Z, KeyCode::Z},
            {GLFW_KEY_LEFT_BRACKET, KeyCode::LeftBracket},
            {GLFW_KEY_BACKSLASH, KeyCode::Backslash},
            {GLFW_KEY_RIGHT_BRACKET, KeyCode::RightBracket},
            {GLFW_KEY_GRAVE_ACCENT, KeyCode::GraveAccent},
            {GLFW_KEY_ESCAPE, KeyCode::Escape},
            {GLFW_KEY_ENTER, KeyCode::Enter},
            {GLFW_KEY_TAB, KeyCode::Tab},
            {GLFW_KEY_BACKSPACE, KeyCode::Backspace},
            {GLFW_KEY_INSERT, KeyCode::Insert},
            {GLFW_KEY_DELETE, KeyCode::DelKey},
            {GLFW_KEY_RIGHT, KeyCode::Right},
            {GLFW_KEY_LEFT, KeyCode::Left},
            {GLFW_KEY_DOWN, KeyCode::Down},
            {GLFW_KEY_UP, KeyCode::Up},
            {GLFW_KEY_PAGE_UP, KeyCode::PageUp},
            {GLFW_KEY_PAGE_DOWN, KeyCode::PageDown},
            {GLFW_KEY_HOME, KeyCode::Home},
            {GLFW_KEY_END, KeyCode::End},
            {GLFW_KEY_CAPS_LOCK, KeyCode::CapsLock},
            {GLFW_KEY_SCROLL_LOCK, KeyCode::ScrollLock},
            {GLFW_KEY_NUM_LOCK, KeyCode::NumLock},
            {GLFW_KEY_PRINT_SCREEN, KeyCode::PrintScreen},
            {GLFW_KEY_PAUSE, KeyCode::Pause},
            {GLFW_KEY_F1, KeyCode::F1},
            {GLFW_KEY_F2, KeyCode::F2},
            {GLFW_KEY_F3, KeyCode::F3},
            {GLFW_KEY_F4, KeyCode::F4},
            {GLFW_KEY_F5, KeyCode::F5},
            {GLFW_KEY_F6, KeyCode::F6},
            {GLFW_KEY_F7, KeyCode::F7},
            {GLFW_KEY_F8, KeyCode::F8},
            {GLFW_KEY_F9, KeyCode::F9},
            {GLFW_KEY_F10, KeyCode::F10},
            {GLFW_KEY_F11, KeyCode::F11},
            {GLFW_KEY_F12, KeyCode::F12},
            {GLFW_KEY_KP_0, KeyCode::KP_0},
            {GLFW_KEY_KP_1, KeyCode::KP_1},
            {GLFW_KEY_KP_2, KeyCode::KP_2},
            {GLFW_KEY_KP_3, KeyCode::KP_3},
            {GLFW_KEY_KP_4, KeyCode::KP_4},
            {GLFW_KEY_KP_5, KeyCode::KP_5},
            {GLFW_KEY_KP_6, KeyCode::KP_6},
            {GLFW_KEY_KP_7, KeyCode::KP_7},
            {GLFW_KEY_KP_8, KeyCode::KP_8},
            {GLFW_KEY_KP_9, KeyCode::KP_9},
            {GLFW_KEY_KP_DECIMAL, KeyCode::KP_Decimal},
            {GLFW_KEY_KP_DIVIDE, KeyCode::KP_Divide},
            {GLFW_KEY_KP_MULTIPLY, KeyCode::KP_Multiply},
            {GLFW_KEY_KP_SUBTRACT, KeyCode::KP_Subtract},
            {GLFW_KEY_KP_ADD, KeyCode::KP_Add},
            {GLFW_KEY_KP_ENTER, KeyCode::KP_Enter},
            {GLFW_KEY_KP_EQUAL, KeyCode::KP_Equal},
            {GLFW_KEY_LEFT_SHIFT, KeyCode::LeftShift},
            {GLFW_KEY_LEFT_CONTROL, KeyCode::LeftControl},
            {GLFW_KEY_LEFT_ALT, KeyCode::LeftAlt},
            {GLFW_KEY_RIGHT_SHIFT, KeyCode::RightShift},
            {GLFW_KEY_RIGHT_CONTROL, KeyCode::RightControl},
            {GLFW_KEY_RIGHT_ALT, KeyCode::RightAlt},
        };

    auto key_it = key_lookup.find(key);

    if (key_it == key_lookup.end()) {
        return KeyCode::Unknown;
    }

    return key_it->second;
}

KeyAction GlfwTranslateKeyAction(int action) {
    if (action == GLFW_PRESS) {
        return KeyAction::Down;
    } else if (action == GLFW_RELEASE) {
        return KeyAction::Up;
    } else if (action == GLFW_REPEAT) {
        return KeyAction::Repeat;
    }

    return KeyAction::Unknown;
}

inline MouseAction GlfwTranslateMouseAction(int action) {
    if (action == GLFW_PRESS) {
        return MouseAction::Down;
    } else if (action == GLFW_RELEASE) {
        return MouseAction::Up;
    }

    return MouseAction::Unknown;
}

inline MouseButton GlfwTranslateMouseButton(int button) {
    if (button < GLFW_MOUSE_BUTTON_6) {
        return static_cast<MouseButton>(button);
    }

    return MouseButton::Unknown;
}

void keyCallback(GLFWwindow* window, int key, int /*scancode*/, int action, int /*mods*/) {
    KeyCode   key_code   = GlfwTranslateKeyCode(key);
    KeyAction key_action = GlfwTranslateKeyAction(action);

    if (auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))) {
        app->inputEvent(KeyInputEvent{key_action, key_code});
    }
}

void cursor_position_callback(GLFWwindow* window, double xpos, double ypos) {
    if (auto* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))) {
        app->inputEvent(MouseButtonInputEvent{
            MouseButton::Unknown,
            MouseAction::Move,
            static_cast<float>(xpos),
            static_cast<float>(ypos)});
    }
}

void setFoucesCallBack(GLFWwindow* window, int focused) {
    if (auto* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))) {
        app->setFocused(focused);
    }
}

void mouseButtonCallBack(GLFWwindow* window, int button, int action, int mods) {
    MouseAction mouseAction = GlfwTranslateMouseAction(action);
    auto        mouseButton = GlfwTranslateMouseButton(button);

    if (auto app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);

        app->inputEvent(MouseButtonInputEvent{
            mouseButton,
            mouseAction,
            static_cast<float>(xpos),
            static_cast<float>(ypos)});
    }
}

VkSurfaceKHR Window::createSurface(Instance& instance) {
    VkSurfaceKHR surface;
    glfwCreateWindowSurface(instance.getHandle(), handle, nullptr, &surface);
    return surface;
}

Window::Window(const WindowProp& prop, Application* app) : prop(prop) {
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
    handle = glfwCreateWindow(prop.extent.width, prop.extent.height, prop.title.c_str(), nullptr, nullptr);
    glfwSetWindowUserPointer(handle, app);
    glfwSetKeyCallback(handle, keyCallback);
    glfwSetMouseButtonCallback(handle, mouseButtonCallBack);
    glfwSetCursorPosCallback(handle, cursor_position_callback);
    glfwSetWindowFocusCallback(handle, setFoucesCallBack);
    glfwSetWindowSizeCallback(handle, [](GLFWwindow* window, int width, int height) {
        if (auto* app = reinterpret_cast<Application*>(glfwGetWindowUserPointer(window))) {
            app->onResize(width, height);
        }
    });
}

//
// Created by 打工人 on 2023/3/11.
//