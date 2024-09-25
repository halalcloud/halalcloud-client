/**
 * Copyright 2014 (c) Baidu, Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except
 * in compliance with the License. You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License
 * is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express
 * or implied. See the License for the specific language governing permissions and limitations under
 * the License.
 */
#ifndef BAIDU_BOS_CPPSDK_BOS_MODEL_ACL_DATA_H
#define BAIDU_BOS_CPPSDK_BOS_MODEL_ACL_DATA_H

namespace Json {
class Value;
}

BEGIN_CPPSDK_NAMESPACE

struct Owner {
    std::string id;
};

struct CurrentTime {
    std::string dateLessThan;
    std::string dateLessThanEquals;
    std::string dateGreaterThan;
    std::string dateGreaterThanEquals;
};

struct Referer {
    std::vector<std::string> stringLike;
    std::vector<std::string> stringEquals;
};

struct Condition {
    Condition() : secureTransport(false) {}
    std::vector<std::string> ipAddress;
    Referer referer;
    CurrentTime currentTime;
    bool secureTransport;
};

struct Grantee {
    std::string id;
};

struct Grant {
    std::vector<Grantee> grantee;
    std::vector<std::string> permission;
    std::vector<std::string> resource;
    std::vector<std::string> notResource;
    Condition condition;
    std::string effect;

    static int serialize_to_json(const std::vector<Grant> &st, std::string &json_str, bool is_bucket = false);
    static int deserialize_to_grants(Json::Value &root, std::vector<Grant> &access_control_list, bool is_bucket = false);
};

END_CPPSDK_NAMESPACE

#endif
