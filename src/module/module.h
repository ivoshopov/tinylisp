
struct module {
	void (*setup) (void);
	void (*teardown) (void);
};

#define MODULE_SECTION __attribute__((section("modules")))

// Begin and end of module list (the linker initialize them)
extern struct module __start_modules;
extern struct module __stop_modules;
