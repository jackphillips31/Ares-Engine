#include "launcher/module_registry.h"

module_registry& module_registry::get()
{
	static module_registry instance;
	return instance;
}

void module_registry::register_module(const std::string& name, module_factory factory)
{
	m_factories[name] = factory;
}

module* module_registry::create(const std::string& name)
{
	auto it = m_factories.find(name);
	return it != m_factories.end() ? it->second() : nullptr;
}