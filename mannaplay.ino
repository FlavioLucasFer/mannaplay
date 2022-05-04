#include <inttypes.h>
#include <Keypad.h>
#include <LinkedList.h>
#include <LiquidCrystal_I2C.h>

#define PRINT(...) Serial.print(__VA_ARGS__)
#define PRINT_L(string_literal) Serial.print(F(string_literal))
#define PRINTLN(...) Serial.println(__VA_ARGS__)
#define PRINTLN_L(string_literal) Serial.println(F(string_literal))

#define IDLE_TIMEOUT 120000

// KEYPAD CONFIG > BEGIN
#define KEYPAD_ROWS 4
#define KEYPAD_COLS 4

#define KEYPAD_ROW_PINS (byte[]){22, 24, 26, 28}
#define KEYPAD_COL_PINS (byte[]){30, 32, 34, 36}

char keypad_keys[KEYPAD_ROWS][KEYPAD_COLS] = {
	{'1', '2', '3', 'A'},
	{'4', '5', '6', 'B'},
	{'7', '8', '9', 'C'},
	{'*', '0', '#', 'D'},
};

Keypad keypad = Keypad(makeKeymap(keypad_keys), KEYPAD_ROW_PINS, KEYPAD_COL_PINS, KEYPAD_ROWS, KEYPAD_COLS);

// KEYPAD CONFIG < END

LiquidCrystal_I2C lcd(0x27, 20, 4);

enum class device_state_t
{
	menu,
	play,
	config_difficulty,

#define MENU_DS device_state_t::menu
#define PLAY_DS device_state_t::play
#define CONFIG_DIFFICULTY_DS device_state_t::config_difficulty
};

enum class game_t
{
	vault,
	think_fast,

#define VAULT_GAME game_t::vault
#define THINK_FAST_GAME game_t::think_fast
};

enum class game_difficulty_t
{
	easy,
	medium,
	hard,
	very_hard,
	elden_ring,

#define EASY_GD game_difficulty_t::easy
#define MEDIUM_GD game_difficulty_t::medium
#define HARD_GD game_difficulty_t::hard
#define VERY_HARD_GD game_difficulty_t::very_hard
#define ELDEN_RING_GD game_difficulty_t::elden_ring
};

device_state_t device_state = MENU_DS;
game_t game = VAULT_GAME;
game_difficulty_t game_difficulty = MEDIUM_GD;

uint8_t chars_quantity = 3;
uint16_t blink_speed = 1000;

uint64_t last_user_interation = 0;

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

void print (uint8_t col, uint8_t line, int message) 
{
	lcd.setCursor(col, line);
	lcd.print(message);	
}

char wait_user_input ()
{
	char key;
	do {
		check_idleness();
		
		key = keypad.getKey();
	} while (key == NO_KEY);
	last_user_interation = millis();
	return key;
}

void check_idleness ()
{
	if ((millis() - last_user_interation) > IDLE_TIMEOUT)
		stand_by();
}

void stand_by() {
	lcd.noBacklight();
	keypad.waitForKey();
	last_user_interation = millis();
	lcd.backlight();
}

const char* get_difficulty_name_in_portuguese ()
{
	switch (game_difficulty) {
		case EASY_GD:
			return "(facil)";
		case HARD_GD:
			return "(dificil)";
		case VERY_HARD_GD:
			return "(muito dific.)";
		case ELDEN_RING_GD:
			return "(elden ring)";
		default:
			return "(medio)";
	}
}

void stop_or_continue ()
{
	lcd.clear();
	print(0, 0, "Deseja continuar?");
	print(0, 1, "Sim = 1 | Nao = 2");

	char key;
	
	do {
		key = wait_user_input();

		if (key == '2')
			device_state = MENU_DS;
	} while (key != '1' && key != '2');

	lcd.clear();
}

void change_difficulty (game_difficulty_t difficulty) 
{
	game_difficulty = difficulty;

	switch (difficulty) {
		case EASY_GD:
			chars_quantity = 3;
			blink_speed = 1000;
			break;
		case HARD_GD:
			chars_quantity = 5;
			blink_speed = 400;
			break;
		case VERY_HARD_GD:
			chars_quantity = 7;
			blink_speed = 300;
			break;
		case ELDEN_RING_GD:
			chars_quantity = 10;
			blink_speed = 100;
			break;
		default:
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

	char key;
		
	do {
		key = wait_user_input();

		switch (key) {
			case '1':
				device_state = PLAY_DS;
				break;
			case '2':
				device_state = CONFIG_DIFFICULTY_DS;
				break;
			default:
				break;
		}
	} while (key != '1' && key != '2');

	lcd.clear();
}

void set_game ()
{
	print(0, 0, "<====== JOGAR =====>");
	print(0, 2, "1 - Lembre a senha");
	print(0, 3, "2 - Pense rapido");

	char key;

	do {
		key = wait_user_input();

		switch (key) {
			case '1':
				game = VAULT_GAME;
				break;
			case '2':
				game = THINK_FAST_GAME;
				break;
			default:
				break;
		}
	} while (key != '1' && key != '2');

	switch (game) {
		case VAULT_GAME:
			vault_game();
			break;
		case THINK_FAST_GAME:
			think_fast_game();
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

	uint8_t key;

	do {
		key = ((uint8_t) wait_user_input()) - 48;

		switch (key) {
			case 1:
				change_difficulty(EASY_GD);
				break;
			case 3:
				change_difficulty(HARD_GD);
				break;
			case 4:
				change_difficulty(VERY_HARD_GD);
				break;
			case 5:
				change_difficulty(ELDEN_RING_GD);
				break;
			default:
				change_difficulty(MEDIUM_GD);
				break;
		}
	} while (key < 1 || key > 5);

	lcd.clear();
	device_state = MENU_DS;
}

void vault_game ()
{
	lcd.clear();
	LinkedList<uint8_t> vault_sequence = LinkedList<uint8_t>();
	LinkedList<uint8_t> user_inputs = LinkedList<uint8_t>();

	while (device_state == device_state_t::play) {
		print(0, 0, "<< Lembre a senha >>");
		print(3, 2, "APERTE QUALQUER");
		print(1, 3, "TECLA PARA INICIAR");

		wait_user_input();
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
			char user_input = wait_user_input();

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

		stop_or_continue();
	}
};

void think_fast_game () 
{
	lcd.clear();

	while (device_state == PLAY_DS) {
		boolean win_state = true;
		uint64_t score = 0;
		uint16_t speed = blink_speed;
		uint8_t last_random_number = 10;

		print(0, 0, "<<  Pense rapido >>");
		print(3, 2, "APERTE QUALQUER");
		print(1, 3, "TECLA PARA INICIAR");

		wait_user_input();
		lcd.clear();

		print(4, 0, "Seja rapido!");
		print(4, 2, "Iniciando em: ");

		delay(1000);
		print(3, 3, "3... ");
		delay(1000);
		print(8, 3, "2... ");
		delay(1000);
		print(13, 3, "1... ");
		delay(1000);

		lcd.clear();

		while (win_state) {
			char key;
			uint64_t previous_millis;
			uint8_t random_number;

			while (true) {
				random_number = random(10);

				if (random_number != last_random_number) {
					last_random_number = random_number;
					break;
				}
			}

			if ((score % 1000) == 0 && speed > (blink_speed / 2)) {
				if (game_difficulty == game_difficulty_t::elden_ring) 
					speed -= 10;
				else 
					speed -= 50;
			}

			digitalWrite(random_number + 2, HIGH);
			previous_millis = millis();

			while ((millis() - previous_millis) < (speed * 2)) {
				key = keypad.getKey();

				if (key != NO_KEY) {
					last_user_interation = millis();
					break;
				}
			}

			if (key == (char) random_number + 48) {
				score += 100;

				lcd.clear();
				print(0, 0, "Pontuacao: ");
				print(0, 1, (int) score);
			}
			else 
				win_state = false;
			
			digitalWrite(random_number + 2, LOW);
		}

		lcd.clear();
		print(4, 1, "Sua pontuacao:");
		print(0, 2, (int) score);
		delay(2000);
		stop_or_continue();
	}
}

void setup ()
{
	Wire.begin();
	Serial.begin(9600);

	pinMode(2, OUTPUT);
	pinMode(3, OUTPUT);
	pinMode(4, OUTPUT);
	pinMode(5, OUTPUT);
	pinMode(6, OUTPUT);
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
	check_idleness();
	switch (device_state) {
		case PLAY_DS:
			set_game();
			break;
		case CONFIG_DIFFICULTY_DS:
			set_difficulty();
			break;
		default:
			menu();
			break;
		}
}
