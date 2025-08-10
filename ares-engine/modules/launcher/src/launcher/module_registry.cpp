#include <ares_launcher_pch.h>
#include "launcher/module_registry.h"

module_registry& module_registry::get()
{
	static module_registry instance;
	return instance;
}

void module_registry::register_module(const std::string& name, module_factory factory)
{
	factories_[name] = factory;
}

module* module_registry::create(const std::string& name)
{
	auto it = factories_.find(name);
	return it != factories_.end() ? it->second() : nullptr;
}