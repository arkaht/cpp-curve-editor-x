#pragma once

namespace curve_editor_x
{
	enum class InputState
	{
		Up,
		Pressed,
		Down,
		Released,
	};

	enum class InputKey
	{
		None,

		LeftClick,
		MiddleClick,
		RightClick,

		Delete,
	};

	struct UserInput
	{
	public:
		UserInput( 
			const InputState state, 
			const InputKey type 
		)
			: state( state ),
			  key( type )
		{}

		bool is_mouse_input() const
		{
			return key == InputKey::LeftClick
				|| key == InputKey::RightClick;
		}

	public:
		InputState state = InputState::Up;
		InputKey key = InputKey::None;
	};
}