#include "package_manifest.h"

#include <cctype>
#include <fstream>
#include <sstream>

namespace {
class JsonReader {
public:
	explicit JsonReader(std::string text) : text_(std::move(text)) {}

	static bool fail(std::string& error, const char* message) {
		error = message;
		return false;
	}

	bool parse(Rivet::PackageManifest& manifest, std::string& error) {
		skip_space();
		if (!consume('{'))
			return fail(error, "manifest must start with an object");

		bool first = true;
		bool hasName = false;
		bool hasVersion = false;
		bool hasWebsiteUrl = false;
		bool hasDescription = false;
		bool hasDependencies = false;
		while (true) {
			skip_space();
			if (consume('}'))
				break;
			if (!first && !consume(','))
				return fail(error, "expected comma between manifest fields");
			first = false;

			std::string key;
			if (!read_string(key) || !consume(':'))
				return fail(error, "expected a manifest field name");

			if (key == "name" || key == "version_number" || key == "website_url" || key == "description") {
				std::string value;
				if (!read_string(value))
					return fail(error, "manifest scalar fields must be strings");
				if (key == "name") {
					hasName = true;
					manifest.name = std::move(value);
				} else if (key == "version_number") {
					hasVersion = true;
					manifest.version = std::move(value);
				} else if (key == "website_url") {
					hasWebsiteUrl = true;
					manifest.websiteUrl = std::move(value);
				} else {
					hasDescription = true;
					manifest.description = std::move(value);
				}
			} else if (key == "dependencies") {
				if (!read_string_array(manifest.dependencies, error))
					return false;
				hasDependencies = true;
			} else if (!skip_value(error)) {
				return false;
			}
		}

		skip_space();
		if (position_ != text_.size())
			return fail(error, "unexpected data after manifest object");
		if (!hasName || !hasVersion || !hasWebsiteUrl || !hasDescription || !hasDependencies)
			return fail(error, "manifest requires name, version_number, website_url, description, and dependencies");
		return true;
	}

private:
	bool consume(char expected) {
		skip_space();
		if (position_ >= text_.size() || text_[position_] != expected)
			return false;
		++position_;
		return true;
	}

	void skip_space() {
		while (position_ < text_.size() && std::isspace(static_cast<unsigned char>(text_[position_])))
			++position_;
	}

	bool read_string(std::string& value) {
		skip_space();
		if (position_ >= text_.size() || text_[position_] != '"')
			return false;
		++position_;
		value.clear();
		while (position_ < text_.size()) {
			const char current = text_[position_++];
			if (current == '"')
				return true;
			if (current != '\\') {
				value.push_back(current);
				continue;
			}
			if (position_ >= text_.size())
				return false;
			const char escaped = text_[position_++];
			switch (escaped) {
			case '"':
				value.push_back('"');
				break;
			case '\\':
				value.push_back('\\');
				break;
			case '/':
				value.push_back('/');
				break;
			case 'b':
				value.push_back('\b');
				break;
			case 'f':
				value.push_back('\f');
				break;
			case 'n':
				value.push_back('\n');
				break;
			case 'r':
				value.push_back('\r');
				break;
			case 't':
				value.push_back('\t');
				break;
			default:
				return false;
			}
		}
		return false;
	}

	bool read_string_array(std::vector<std::string>& values, std::string& error) {
		if (!consume('['))
			return fail(error, "dependencies must be an array");
		bool first = true;
		while (true) {
			skip_space();
			if (consume(']'))
				return true;
			if (!first && !consume(','))
				return fail(error, "expected comma between dependencies");
			first = false;
			std::string dependency;
			if (!read_string(dependency))
				return fail(error, "dependencies must contain strings");
			values.push_back(std::move(dependency));
		}
	}

	bool skip_value(std::string& error) {
		skip_space();
		if (position_ >= text_.size())
			return fail(error, "missing manifest value");
		if (text_[position_] == '"') {
			std::string ignored;
			return read_string(ignored);
		}
		if (text_[position_] != '{' && text_[position_] != '[')
			return skip_literal();
		const char opening = text_[position_++];
		const char closing = opening == '{' ? '}' : ']';
		int depth = 1;
		bool quoted = false;
		bool escaped = false;
		while (position_ < text_.size() && depth != 0) {
			const char current = text_[position_++];
			if (quoted) {
				if (escaped)
					escaped = false;
				else if (current == '\\')
					escaped = true;
				else if (current == '"')
					quoted = false;
				continue;
			}
			if (current == '"')
				quoted = true;
			else if (current == opening)
				++depth;
			else if (current == closing)
				--depth;
		}
		return depth == 0 || fail(error, "unterminated manifest value");
	}

	bool skip_literal() {
		while (position_ < text_.size() && text_[position_] != ',' && text_[position_] != '}')
			++position_;
		return true;
	}

	std::string text_;
	std::size_t position_ = 0;
};
} // namespace

namespace Rivet {
bool ReadPackageManifest(const std::filesystem::path& path, PackageManifest& manifest, std::string& error) {
	std::ifstream input(path, std::ios::binary);
	if (!input)
		return JsonReader::fail(error, "could not open manifest.json");
	std::ostringstream buffer;
	buffer << input.rdbuf();
	return JsonReader(buffer.str()).parse(manifest, error);
}
} // namespace Rivet
