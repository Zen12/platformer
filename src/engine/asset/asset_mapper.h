#include <iostream>
#include <map>
#include <string>

class AssetMapper
{
    private:
        std::map<std::string, std::string> _map;

    public:
    
        void Add(const std::string& path, const std::string& hash) const
        {
            _map[hash] = path;
        }

        const std::string GetPath(const std::string& hash)
        {
            return _map[hash];
        }
    
};