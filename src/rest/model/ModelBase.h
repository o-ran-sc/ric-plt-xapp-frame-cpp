#ifndef XAPP_MODEL_ModelBase_H
#define XAPP_MODEL_ModelBase_H
#include <iostream>
#include <string.h>
#include <nlohmann/json.hpp>
#include <nlohmann/json-schema.hpp>
#include <type_traits>

using nlohmann::json_schema::json_validator;
using json = nlohmann::json;

namespace xapp {
namespace model {

std::invalid_argument invalid_parameter("Invalid Json Input");

template<typename ModelType>
bool _validate(const ModelType model) {
    json _json = model;
    json_validator validator;
    validator.set_root_schema(model.validator_schema);

    try {
        validator.validate(_json);
    } catch (const std::exception& e) {
        std::cerr << "Struct Validation failed, here is why: " << e.what() << "\n";
        throw;
    }
    return true;
}

struct ModelBase {
    json validator_schema = R"(
    {
        "$schema": "http://json-schema.org/draft-07/schema#",
        "title": "ModelBase"
    })"_json;

    bool validate_() {
        return _validate(std::move(*this));
    }

    void validate_json(const json& _json) {
        json_validator validator;
        validator.set_root_schema(get_validator_schema());
        try {
            validator.validate(_json);
        } catch (const std::exception& e) {
            throw;
        }
        return;
    }

    virtual json get_validator_schema() const { return validator_schema; }
};

void from_json(const json& j, ModelBase& ref) {
    return;
}
void to_json(json& j, const ModelBase& ref) {
    return;
}

} /*model*/
} /*xapp*/
#endif /*XAPP_MODEL_ModelBase_H*/
