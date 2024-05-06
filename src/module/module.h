
struct module {
	void (*setup) (void);
	void (*teardown) (void);
};

#define MODULE_SECTION __attribute__((section("modules")))
