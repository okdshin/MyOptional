#include <iostream>
#include "../optional.hpp"

template<typename T>
void print_opt(my::optional<T> const& opt) {
	if(opt) {
		std::cout << "opt is engaged. value is " << *opt << std::endl;
	}
	else {
		std::cout << "opt is not engaged." << std::endl;
	}
}
int main() {
	my::optional<int> int_opt;
	print_opt(int_opt);
	int_opt = 42;
	print_opt(int_opt);
	int_opt = my::optional<int>();
	print_opt(int_opt);
}
