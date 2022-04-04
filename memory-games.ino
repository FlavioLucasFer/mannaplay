#include <Keypad.h>
#include <inttypes.h>
#include <LinkedList.h>
#include <LiquidCrystal_I2C.h>

// KEYPAD CONFIG > BEGIN

const byte KEYPAD_ROWS = 4;
const byte KEYPAD_COLS = 4;

char keypad_keys[KEYPAD_ROWS][KEYPAD_COLS] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'},
};

byte keypad_row_pins[KEYPAD_ROWS] = {22, 24, 26, 28};
byte keypad_col_pins[KEYPAD_COLS] = {30, 32, 34, 36};

Keypad keypad = Keypad(makeKeymap(keypad_keys), keypad_row_pins, keypad_col_pins, KEYPAD_ROWS, KEYPAD_COLS);

// KEYPAD CONFIG < END

LiquidCrystal_I2C lcd(0x27, 20, 4);

enum class main_menu_t 
{
	menu,
	play,
	config_difficulty,
};

enum class game_t
{
	vault,
};

enum class game_difficulty_t
{
	easy,
	medium,
	hard,
	very_hard,
	elden_ring,
};

main_menu_t main_menu = main_menu_t::menu;
game_t game = game_t::vault;
game_difficulty_t game_difficulty = game_difficulty_t::medium;

uint8_t chars_quantity = 3;
uint16_t blink_speed = 1000;

void print (uint8_t col, uint8_t line, const char* message) 
{
	lcd.setCursor(col, line);
	lcd.print(message);	
}

void print (uint8_t col, uint8_t line, char message) 
{
	lcd.setCursor(col, line);
	lcd.print(message);	
}

const char* get_difficulty_name_in_portuguese()
{
	switch (game_difficulty)
	{
	case game_difficulty_t::easy:
		return "(facil)";

	case game_difficulty_t::hard:
		return "(dificil)";

	case game_difficulty_t::very_hard:
		return "(muito dific.)";

	case game_difficulty_t::elden_ring:
		return "(elden ring)";
	
	default:
		return "(medio)";
	}
}

void change_difficulty (game_difficulty_t difficulty) 
{
	switch (difficulty) {
	case game_difficulty_t::easy:
		game_difficulty = game_difficulty_t::easy;
		chars_quantity = 3;
		blink_speed = 1000;
		break;

	case game_difficulty_t::hard:
		game_difficulty = game_difficulty_t::hard;
		chars_quantity = 5;
		blink_speed = 400;
		break;

	case game_difficulty_t::very_hard:
		game_difficulty = game_difficulty_t::very_hard;
		chars_quantity = 7;
		blink_speed = 300;
		break;

	case game_difficulty_t::elden_ring:
		game_difficulty = game_difficulty_t::elden_ring;
		chars_quantity = 10;
		blink_speed = 100;
		break;
	
	default:
		game_difficulty = game_difficulty_t::medium;
		chars_quantity = 3;
		blink_speed = 500;
		break;
	}
}

void menu ()
{
	print(0, 0, "<= MENU PRINCIPAL =>");
	print(0, 1, get_difficulty_name_in_portuguese());
	print(0, 2, "1 - Jogar");
	print(0, 3, "2 - Escolher dific.");

	char key = '3';
		
	while (key != '1' && key != '2') {
		key = keypad.waitForKey();

		switch (key)
		{
		case '1':
			main_menu = main_menu_t::play;
			break;

		case '2':
			main_menu = main_menu_t::config_difficulty;
			break;
		
		default:
			break;
		}
	}

	lcd.clear();
}

void set_game ()
{
	print(0, 0, "<====== JOGAR =====>");
	print(0, 2, "1 - Jogo do cofre");

	char key = '3';

	while (key != '1') {
		key = keypad.waitForKey();

		switch (key)
		{
		case '1':
			game = game_t::vault;
			break;
		
		default:
			break;
		}
	}

	switch (game)
	{
	case game_t::vault:
		vault_game();
		break;
	
	default:
		vault_game();
		break;
	}

	lcd.clear();
}

void set_difficulty ()
{
	print(0, 0, "1 - Facil |2 - Medio");
	print(0, 1, "3 - Dificil");
	print(0, 2, "4 - Muito dificil");
	print(0, 3, "5 - Elden Ring");

	char key = '6';
	uint8_t key_number = (uint8_t) key - 48;

	while (key_number < 1 || key_number > 5) {
		key = keypad.waitForKey();
		key_number = (uint8_t) key - 48;

		switch (key_number)
		{
		case 1:
			change_difficulty(game_difficulty_t::easy);
			break;

		case 3:
			change_difficulty(game_difficulty_t::hard);
			break;

		case 4:
			change_difficulty(game_difficulty_t::very_hard);
			break;

		case 5:
			change_difficulty(game_difficulty_t::elden_ring);
			break;
		
		default:
			change_difficulty(game_difficulty_t::medium);
			break;
		}
	}

	lcd.clear();
	main_menu = main_menu_t::menu;
}

void vault_game ()
{
	lcd.clear();
	LinkedList<uint8_t> vault_sequence = LinkedList<uint8_t>();
	LinkedList<uint8_t> user_inputs = LinkedList<uint8_t>();

	while (main_menu == main_menu_t::play) {
		print(0, 0, " << Jogo do cofre >>");
		print(3, 2, "APERTE QUALQUER");
		print(1, 3, "TECLA PARA INICIAR");

		keypad.waitForKey();
		lcd.clear();

		boolean win_state = true;

		print(2, 0, "Memorize a senha");
		print(1, 1, "para abrir o cofre!");
		print(4, 2, "Iniciando em: ");

		delay(1000);
		print(3, 3, "3... ");
		delay(1000);
		print(8, 3, "2... ");
		delay(1000);
		print(13, 3, "1... ");
		delay(1000);

		for (uint8_t i = 1; i <= chars_quantity; i++) {
			uint8_t random_number = random(10);

			vault_sequence.add(random_number);

			digitalWrite(random_number + 2, HIGH);
			delay(blink_speed);
			digitalWrite(random_number + 2, LOW);
			delay(blink_speed);
		}

		lcd.clear();
		print(0, 0, "Digite a senha: ");

		for (uint8_t i = 1; i <= chars_quantity; i++) {
			char user_input = keypad.waitForKey();

			print(i - 1, 1, user_input);
			user_inputs.add((uint8_t)user_input - 48);
		}

		print(0, 3, "Validando senha.");
		delay(300);
		print(16, 3, ".");
		delay(300);
		print(17, 3, ".");
		delay(400);

		for (uint8_t i = 0; i <= vault_sequence.size(); i++)
			if (vault_sequence[i] != user_inputs[i]) {
				win_state = false;
				break;
			}

		lcd.clear();

		print(win_state ? 3 : 2, 1, win_state ? "Senha correta" : "Senha incorreta");
		print(4, 2, win_state ? "Voce ganhou!" : "Voce perdeu!");

		vault_sequence.clear();
		user_inputs.clear();

		delay(1000);

		lcd.clear();
		print(0, 0, "Deseja continuar?");
		print(0, 1, "Sim = 1 | Nao = 2");

		char key = '3';
		
		while (key != '1' && key != '2') {
			key = keypad.waitForKey();

			if (key == '2')
				main_menu = main_menu_t::menu;
		}

		lcd.clear();
	}
};

void setup ()
{
	Wire.begin();
	Serial.begin(9600);

	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(7, OUTPUT);
	pinMode(8, OUTPUT);
	pinMode(9, OUTPUT);
	pinMode(10, OUTPUT);
	pinMode(11, OUTPUT);

	lcd.init();
	lcd.backlight();

	randomSeed(analogRead(0));
}

void loop ()
{
	switch (main_menu)
	{
	case main_menu_t::play:
		set_game();
		break;
	
	case main_menu_t::config_difficulty:
		set_difficulty();
		break;

	default:
		menu();
		break;
	}
}
