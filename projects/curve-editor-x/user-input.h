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

		//  Left mouse button
		LeftClick,
		//  Middle mouse button
		MiddleClick,
		//  Right mouse button
		RightClick,

		//  F1 key
		F1,
		//  F2 key
		F2,
		//  F3 key
		F3,

		//  TAB key
		Mode,
		//  F key
		Focus,
		//  Suppr key
		Delete,
	};

	//  TODO: Add a way to check for combination keys such as
	//        Ctrl, Shift & Alt using the InputKey enum.
	struct UserInput
	{
	public:
		UserInput( 
			const InputState state, 
			const InputKey key 
		)
			: state( state ),
			  key( key )
		{}

		bool is( const InputKey key ) const
		{
			return this->key == key;
		}
		bool is( const InputKey key, const InputState state ) const
		{
			return this->key == key
			    && this->state == state;
		}

		bool is_mouse_input() const
		{
			return key == InputKey::LeftClick
				|| key == InputKey::RightClick;
		}

		bool is_pressed() const
		{
			return state == InputState::Pressed;
		}
		bool is_released() const
		{
			return state == InputState::Released;
		}

	public:
		InputState state = InputState::Up;
		InputKey key = InputKey::None;
	};
}