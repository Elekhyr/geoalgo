#include "Importer.hpp"
#include "OffImporter.hpp"

Ez::Mesh Ez::Importer::ReadFromFile(const std::string& path, Postprocessing pp)
{
	const auto it = path.find_last_of('.');
	if (it == std::string::npos)
	{
		throw std::exception("format not recognized");
	}

	const auto format = path.substr(it, path.length());
	
	if (format == "off")
	{
		return OffImporter().ReadFromFile(path, pp);
	}

	throw std::exception(("format" + format + " not supported : \n Supported formats are : .off").c_str());
}
