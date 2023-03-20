
#ifndef JOY_INPUT_H
#define JOY_INPUT_H

typedef enum key_id
{
    KEY_ID_NULL,
    KEY_ID_ESC,
    KEY_ID_F1,
    KEY_ID_F2,
    KEY_ID_F3,
    KEY_ID_F4,
    KEY_ID_F5,
    KEY_ID_F6,
    KEY_ID_F7,
    KEY_ID_F8,
    KEY_ID_F9,
    KEY_ID_F10,
    KEY_ID_F11,
    KEY_ID_F12,
    KEY_ID_GRAVEACCENT,
    KEY_ID_0,
    KEY_ID_1,
    KEY_ID_2,
    KEY_ID_3,
    KEY_ID_4,
    KEY_ID_5,
    KEY_ID_6,
    KEY_ID_7,
    KEY_ID_8,
    KEY_ID_9,
    KEY_ID_BACKSPACE,
    KEY_ID_DELETE,
    KEY_ID_TAB,
    KEY_ID_A,
    KEY_ID_B,
    KEY_ID_C,
    KEY_ID_D,
    KEY_ID_E,
    KEY_ID_F,
    KEY_ID_G,
    KEY_ID_H,
    KEY_ID_I,
    KEY_ID_J,
    KEY_ID_K,
    KEY_ID_L,
    KEY_ID_M,
    KEY_ID_N,
    KEY_ID_O,
    KEY_ID_P,
    KEY_ID_Q,
    KEY_ID_R,
    KEY_ID_S,
    KEY_ID_T,
    KEY_ID_U,
    KEY_ID_V,
    KEY_ID_W,
    KEY_ID_X,
    KEY_ID_Y,
    KEY_ID_Z,
    KEY_ID_SPACE,
    KEY_ID_ENTER,
    KEY_ID_SHIFT,
    KEY_ID_ALT,
    KEY_ID_CTRL,
    KEY_ID_UP,
    KEY_ID_LEFT,
    KEY_ID_DOWN,
    KEY_ID_RIGHT,
    
    KEY_ID_MAX,
} key_id;

typedef enum mouse_id
{
    MOUSE_ID_LEFT,
    MOUSE_ID_RIGHT,
    MOUSE_ID_MIDDLE,
    
    MOUSE_ID_MAX,
} mouse_id;

enum
{
    INPUT_DOWN = (1<<0),
    INPUT_PRESSED = (1<<1),
    INPUT_RELEASED = (1<<2),
};

typedef enum modifier
{
    MODIFIER_CTRL = (1<<0),
    MODIFIER_ALT = (1<<1),
    MODIFIER_SHIFT = (1<<2),
} modifier;

typedef struct input_state
{
    u8 keyStates[KEY_ID_MAX];
    u8 mouseStates[MOUSE_ID_MAX];
    u8 modifiers;
    v2 mousePos;
    v2 mouseOffset;
    v2 mouseScroll;
} input_state;

internal input_state *
InitInputState()
{
    input_state *inputState = malloc(sizeof(input_state));
    memset(inputState, 0, sizeof(input_state));
    
    return inputState;
}

// TODO(ajeej): move these into a c file and only include in app code
internal inline u32
GetKeyState_(input_state *inputState, key_id key, u8 state)
{
    return inputState->keyStates[key] & state;
}

internal inline u32
GetKeyStateMod(input_state *inputState, key_id key, u8 state,
               modifier mod)
{
    return (inputState->keyStates[key] & state) && (inputState->modifiers & mod);
}

internal inline u32
GetMouseState(input_state *inputState, mouse_id btn, u8 state)
{
    return inputState->mouseStates[btn] & state;
}

internal inline u32
GetMouseStateMod(input_state *inputState, mouse_id btn, u8 state,
                 modifier mod)
{
    return (inputState->mouseStates[btn] & state) && (inputState->modifiers & mod);
}

#endif //JOY_INPUT_H
