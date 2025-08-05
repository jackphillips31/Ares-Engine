#ifndef ARES_LAUNCHER_MODULE_REGISTRY_H
#define ARES_LAUNCHER_MODULE_REGISTRY_H
#include <string>
#include <unordered_map>
#include <functional>

class module
{
public:
	virtual ~module() = default;
	virtual void init() = 0;
};

using module_factory = std::function<module*()>;

class module_registry
{
public:
	static module_registry& get();

	void register_module(const std::string& name, module_factory factory);

	module* create(const std::string& name);

private:
	std::unordered_map<std::string, module_factory> factories_;
};

#endif // ARES_LAUNCHER_MODULE_REGISTRY_H