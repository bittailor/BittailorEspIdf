#include <iostream>
#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

constexpr const char* TAG = "BtCoreUnitTest";

int catchMain(int argc, const char* argv[] ) {
    Catch::Session session;
    int result = session.run(argc, argv);
    std::cout <<  std::endl;
    if(result == 0) {
    	std::cout << "************" << std::endl;
    	std::cout << "** PASSED **" << std::endl;
    	std::cout << "************" << std::endl;
    } else {
    	std::cout << "!!!!!!!!!!!!" << std::endl;
    	std::cout << "!! FAILED !!" << std::endl;
    	std::cout << "!!!!!!!!!!!!" << std::endl;
    }
    std::cout <<  std::endl;
    return result;
}

int main(int argc, const char** argv) {
    return catchMain(argc, argv);
}
