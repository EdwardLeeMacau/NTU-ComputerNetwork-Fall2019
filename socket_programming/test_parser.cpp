#include "gtest/gtest.h"
#include "ping.h"

TEST(parser, initOpts) {
    OPT_S opt;
    init_opt(opt);

    EXPECT_EQ(opt.targets, 0);
    EXPECT_EQ(opt.numbers, -1);
    EXPECT_EQ(opt.timeout, -1);
}

TEST(parser, VaildCase) {
    OPT_S opt;
    int argc; 
    char** argv;

	opt.hints.ai_family   = AF_UNSPEC;			// IPV4 or IPV6
	opt.hints.ai_socktype = SOCK_STREAM;		// TCP Stream Socket
	opt.hints.ai_flags    = AI_PASSIVE;			// My IP Address

    init_opt(opt);
    argc = 2; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = (char*)"./ping";
    argv[1] = (char*)"127.0.0.1:8888";
    EXPECT_EQ(parse(opt, argc, argv, opt.res, opt.hints), PARSE_SUCCESS);
    free(argv);
}

TEST(parser, InvaildCase) {
    OPT_S opt;
    int argc; 
    char** argv;

    opt.hints.ai_family   = AF_UNSPEC;			// IPV4 or IPV6
	opt.hints.ai_socktype = SOCK_STREAM;		// TCP Stream Socket
	opt.hints.ai_flags    = AI_PASSIVE;			// My IP Address

    init_opt(opt);
    argc = 1; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = (char*)"./ping";
    EXPECT_NE(parse(opt, argc, argv, opt.res, opt.hints), PARSE_SUCCESS);
    free(argv);
    freeaddrinfo(opt.res);

    init_opt(opt);
    argc = 2; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = (char*)"./ping";
    argv[1] = (char*)"-n";
    EXPECT_EQ(parse(opt, argc, argv, opt.res, opt.hints), PARSE_MISSING_ARGS);
    free(argv);
    freeaddrinfo(opt.res);

    init_opt(opt);
    argc = 3; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = (char*)"./ping";
    argv[1] = (char*)"-n";
    argv[2] = (char*)"-128";
    EXPECT_EQ(parse(opt, argc, argv, opt.res, opt.hints), PARSE_INVAILD_NUMBER);
    free(argv);
    freeaddrinfo(opt.res);

    init_opt(opt);
    argc = 2; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = (char*)"./ping";
    argv[1] = (char*)"-t";
    EXPECT_EQ(parse(opt, argc, argv, opt.res, opt.hints), PARSE_MISSING_ARGS);
    free(argv);
    freeaddrinfo(opt.res);

    init_opt(opt);
    argc = 3; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = (char*)"./ping";
    argv[1] = (char*)"-t";
    argv[2] = (char*)"-128";
    EXPECT_EQ(parse(opt, argc, argv, opt.res, opt.hints), PARSE_INVAILD_TIMEOUT);
    free(argv);
    freeaddrinfo(opt.res);

    /*
    init_opt(opt);
    argc = 3; argv = (char**)malloc(argc * sizeof(char*)); 
    argv[0] = "./ping";
    argv[1] = "-n";
    argv[2] = "-n";
    EXPECT_EQ(parse(opt, argc, argv, res, hints), PARSE_INVAILD_NUM);
    free(argv);
    freeaddrinfo(opt.res);
    */
}