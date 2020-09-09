#include <console.h>
#include <halidestdlib.h>

void writeAbout(FrameBuffer::Writer &);
void writeString_man(FrameBuffer::Writer &);
void calculator(FrameBuffer::Writer &, hldstd::string &expression);

int init_console(FrameBuffer::Writer &p, Window &win) {
	p.writeString("Now running console instance : \n");
	unsigned int loop = 1;
	int _id = 0;
	char *command;
	while (loop) {

		p.writeString("");
		p.writeString(" root", FrameBuffer::Colours::RED);
		p.writeString("@");
		p.writeString("halideOS $ ");

		hldstd::string command = KEYBOARD_DRIVER::readInput(p);

		if (hldstd::stringCompare(command.c_ptr(), "help")) {
			writeString_man(p);
		}

		else if (hldstd::stringCompare(command.c_ptr(), "about")) {
			writeAbout(p);
		}

		else if (hldstd::stringCompare(command.c_ptr(), "calculator")) {
			p.writeString(" HalideOS - Basic Calculator\n");
			p.writeString(" Enter an Expression (only +, -, *, /): ");
			hldstd::string c = KEYBOARD_DRIVER::readInput(p);
			calculator(p, c);
		}

		else if (hldstd::stringCompare(command.c_ptr(), "clear")) {
			p.clearLine(win.m_y1 + 1, win.m_y2 - 1);
		}

		else if (hldstd::stringCompare(command.c_ptr(), "switch console")) {
			p.writeString("switchched console\n");
			p.clearLine(win.m_y1 + 1, win.m_y2 - 1); // Use this line of code till we impliment local state storage
			_id = 1;
			loop = 0;
			break;
		}

		else if (hldstd::stringCompare(command.c_ptr(), "greet")) {
			p.writeString(" Hello User, Welcome to HalideOS\n");
		}

		else if (hldstd::stringCompare(command.c_ptr(), "exit")) {
			loop = 0;
		}

		else {
			p.writeString(" Invalid command\n"); // For info on how to set color codes please visit that website
		}
	}
	p.writeString("Exiting console\n");
	return _id;
}

inline void writeAbout(FrameBuffer::Writer &p) {
	p.writeString("\n     HalideOS v1.0 \n", FrameBuffer::Colours::LIGHT_BLUE);
	p.writeString("     An experimental OS by DSC KIIT \n");
	p.writeString("     github.com/DSC-KIIT/project-halide \n", FrameBuffer::Colours::GREEN);
	p.writeString("     Developed by Ujjwal Shekhawat and Junaid Rahim\n\n");
}

inline void writeString_man(FrameBuffer::Writer &p) {
	p.writeString(" List of Commands: \n", FrameBuffer::Colours::GREEN);
	p.writeString("    about\n", FrameBuffer::Colours::WHITE);
	p.writeString("    help\n", FrameBuffer::Colours::WHITE);
	p.writeString("    greet\n", FrameBuffer::Colours::WHITE);
	p.writeString("    calculator\n", FrameBuffer::Colours::WHITE);
	p.writeString("    clear\n", FrameBuffer::Colours::WHITE);
	p.writeString("    switch console\n", FrameBuffer::Colours::WHITE);
}

int precedence(char c) {
	switch (c) {
	case '+':
		return 1;
		break;
	case '-':
		return 1;
		break;
	case '*':
		return 2;
		break;
	case '/':
		return 2;
		break;
	default:
		return -1;
		break;
	}
}

bool isValidOperator(char c) {
	if (c == '+' || c == '-' || c == '*' || c == '/' || c == '(' || c == ')') return true;
	else
		return false;
}

void calculator(FrameBuffer::Writer &p, hldstd::string &expression) {
	int s = expression.size();
	hldstd::stack<char> st(s + 2, expression.at(0));

	char postfix[s + 10];
	int index = 0;

	bool validExpression = true;
	int op_count = 0;

	for (int i = 0; i < s - 1; i++) {
		char e = expression.at(i);

		if ((e >= 48) && (e <= 57)) { // if integer
			postfix[index] = e;
			index++;
		}

		else if (isValidOperator(e)) {
			postfix[index] = ',';
			index++;

			while (!(st.isEmpty()) && (precedence(st.top()) >= precedence(e))) {
				char c = st.top();
				st.pop();
				postfix[index] = c;
				index++;
			}
			st.push(e);
			op_count++;
		}

		else {
			validExpression = false;
			break;
		}
	}

	while (!(st.isEmpty())) {
		char c = st.top();
		st.pop();
		postfix[index] = c;
		index++;
	}

	if (validExpression) {
		hldstd::stack<int> st2(s, 0);
		postfix[s + op_count - 1] = '\0';

		char buff[s];
		int index = 0;

		for (int i = 0; postfix[i] != '\0'; i++) {
			if (postfix[i] == ',') {
				hldstd::string int_val = &buff[0];
				st2.push(int_val.to_int());
				index = 0;

			} else if (isValidOperator(postfix[i])) {
				if (postfix[i + 1] == '\0') {
					hldstd::string int_val = &buff[0];
					st2.push(int_val.to_int());
				}


				int a = st2.top();
				st2.pop();
				int b = st2.top();
				st2.pop();

				if (postfix[i] == '+') {
					int val = a+b;
					st2.push(val);
				} else if (postfix[i] == '-') {
					int val = b-a;
					st2.push(val);
				} else if (postfix[i] == '*') {
					int val = a*b;
					st2.push(val);
				} else if (postfix[i] == '/') {
					int val = b/a;
					st2.push(val);
				}

			} else {
				buff[index] = postfix[i];
				buff[index + 1] = '\0';
				index++;
			}
		}

		hldstd::string ans = st2.top();
		p.writeString(" Answer: ");
		p.writeString(ans.c_ptr());
		p.writeString("\n");

	} else {
		p.writeString("Invalid Expression\n");
	}
}