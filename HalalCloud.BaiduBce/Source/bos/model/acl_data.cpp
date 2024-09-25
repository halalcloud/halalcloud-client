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

#include <sstream>
#include <vector>
#include <json/json.h>

#include "bcesdk/common/common.h"
#include "bcesdk/util/util.h"
#include "bcesdk/bos/model/acl_data.h"

BEGIN_CPPSDK_NAMESPACE

int Grant::serialize_to_json(
        const std::vector<Grant> &st,
        std::string &json_str,
        bool is_bucket) {
    try {
        Json::Value root;
        for (const auto &grant : st) {
            Json::Value jgrant;
            if (!grant.grantee.empty()) {
                for (const auto &each : grant.grantee) {
                    Json::Value jgrantee;
                    jgrantee["id"] = each.id.c_str();
                    jgrant["grantee"].append(jgrantee);
                }
            }

            if (!grant.permission.empty()) {
                Json::Value jpermission;
                for (const auto &each : grant.permission) {
                    jpermission.append(each.c_str());
                }
                jgrant["permission"] = jpermission;
            }

            // just adapt for bucket acl
            if (is_bucket) {
                if (!grant.resource.empty()) {
                    Json::Value jresource;
                    for (const auto &each : grant.resource) {
                        jresource.append(each.c_str());
                    }
                    jgrant["resource"] = jresource;
                }

                if (!grant.notResource.empty()) {
                    Json::Value jnotResource;
                    for (const auto &each : grant.notResource) {
                        jnotResource.append(each.c_str());
                    }
                    jgrant["notResource"] = jnotResource;
                }

                // condition
                if (!grant.condition.referer.stringEquals.empty() ||
                    !grant.condition.referer.stringLike.empty() ||
                    !grant.condition.ipAddress.empty() ||
                    !grant.condition.currentTime.dateGreaterThan.empty() ||
                    !grant.condition.currentTime.dateGreaterThanEquals.empty() ||
                    !grant.condition.currentTime.dateLessThan.empty() ||
                    !grant.condition.currentTime.dateLessThanEquals.empty() ||
                    grant.condition.secureTransport) {
                    // condition
                    Json::Value &jcondition = jgrant["condition"];
                    if (!grant.condition.ipAddress.empty()) {
                        Json::Value jipAddress;
                        for (const auto &each : grant.condition.ipAddress) {
                            jipAddress.append(each.c_str());
                        }
                        jcondition["ipAddress"] = jipAddress;
                    }

                    if (!grant.condition.referer.stringEquals.empty() ||
                        !grant.condition.referer.stringLike.empty()) {
                        Json::Value &jreferer = jcondition["referer"];
                        if (!grant.condition.referer.stringEquals.empty()) {
                            Json::Value jstringEquals;
                            for (const auto &each : grant.condition.referer.stringEquals) {
                                jstringEquals.append(each.c_str());
                            }
                            jreferer["stringEquals"] = jstringEquals;
                        }
                        if (!grant.condition.referer.stringLike.empty()) {
                            Json::Value jstringLike;
                            for (const auto &each : grant.condition.referer.stringLike) {
                                jstringLike.append(each.c_str());
                            }
                            jreferer["stringLike"] = jstringLike;
                        }
                    }

                    if (!grant.condition.currentTime.dateGreaterThan.empty() ||
                        !grant.condition.currentTime.dateGreaterThanEquals.empty() ||
                        !grant.condition.currentTime.dateLessThan.empty() ||
                        !grant.condition.currentTime.dateLessThanEquals.empty()) {
                        Json::Value &jcurrentTime = jcondition["currentTime"];
                        if (!grant.condition.currentTime.dateGreaterThan.empty()) {
                            jcurrentTime["dateGreaterThan"] =
                                    grant.condition.currentTime.dateGreaterThan.c_str();
                        }
                        if (!grant.condition.currentTime.dateGreaterThanEquals.empty()) {
                            jcurrentTime["dateGreaterThanEquals"] =
                                    grant.condition.currentTime.dateGreaterThanEquals.c_str();
                        }
                        if (!grant.condition.currentTime.dateLessThan.empty()) {
                            jcurrentTime["dateLessThan"] =
                                    grant.condition.currentTime.dateLessThan.c_str();
                        }
                        if (!grant.condition.currentTime.dateLessThanEquals.empty()) {
                            jcurrentTime["dateLessThanEquals"] =
                                    grant.condition.currentTime.dateLessThanEquals.c_str();
                        }
                    }
                    if (grant.condition.secureTransport) {
                        jcondition["secureTransport"] = grant.condition.secureTransport;
                    }
                }

                if (!grant.effect.empty()) {
                    jgrant["effect"] = grant.effect.c_str();
                }
            }
            root["accessControlList"].append(jgrant);
        }
        Json::FastWriter writer;
        json_str = writer.write(root);
    } catch (...) {
        LOGF(ERROR, "serialize access_control_list to json exceptional!");
        return RET_ILLEGAL_OPERATION;
    }
    return RET_OK;
}

int Grant::deserialize_to_grants(
        Json::Value &root,
        std::vector<Grant> &access_control_list,
        bool is_bucket) {
    try {
        Json::Value &arr = root["accessControlList"];
        LOGF(INFO, "origin json str access_control_list count:%d", arr.size());
        access_control_list.clear();
        for (unsigned int i = 0; i < arr.size(); i++) {
            Json::Value &access_control = arr[i];
            Grant ac;

            Json::Value &grantee = access_control["grantee"];
            ac.grantee.clear();
            for (unsigned int j = 0; j < grantee.size(); ++j) {
                Grantee ge;
                ge.id = grantee[j]["id"].asString();
                ac.grantee.push_back(ge);
            }

            Json::Value &permission = access_control["permission"];
            for (unsigned int j = 0; j < permission.size(); ++j) {
                ac.permission.push_back(permission[j].asString());
            }

            // just adapt for bucket acl
            if (is_bucket) {
                Json::Value &resource = access_control["resource"];
                for (unsigned int j = 0; j < resource.size(); ++j) {
                    ac.resource.push_back(resource[j].asString());
                }

                Json::Value &notResource = access_control["notResource"];
                for (unsigned int j = 0; j < notResource.size(); ++j) {
                    ac.notResource.push_back(notResource[j].asString());
                }

                // condition
                // if (!access_control["condition"].isNull()) {
                if (access_control.isMember("condition")) {
                    Json::Value &condition = access_control["condition"];

                    Json::Value &ipAddress = condition["ipAddress"];
                    for (unsigned int j = 0; j < ipAddress.size(); ++j) {
                        ac.condition.ipAddress.push_back(ipAddress[j].asString());
                    }

                    // if (!condition["referer"].isNull()) {
                    if (condition.isMember("referer")) {
                        Json::Value &referer = condition["referer"];
                        Json::Value &stringLike = referer["stringLike"];
                        for (unsigned int j = 0; j < stringLike.size(); ++j) {
                            ac.condition.referer.stringLike.push_back(stringLike[j].asString());
                        }
                        Json::Value &stringEquals = referer["stringEquals"];
                        for (unsigned int j = 0; j < stringEquals.size(); ++j) {
                            ac.condition.referer.stringEquals.push_back(stringEquals[j].asString());
                        }
                    }

                    // if (!condition["currentTime"].isNull()) {
                    if (condition.isMember("currentTime")) {
                        Json::Value &currentTime = condition["currentTime"];
                        // if (!currentTime["dateLessThan"].isNull()) {
                        if (currentTime.isMember("dateLessThan")) {
                            ac.condition.currentTime.dateLessThan =
                                    currentTime["dateLessThan"].asString();
                        }
                        if (currentTime.isMember("dateLessThanEquals")) {
                            ac.condition.currentTime.dateLessThanEquals =
                                    currentTime["dateLessThanEquals"].asString();
                        }
                        if (currentTime.isMember("dateGreaterThan")) {
                            ac.condition.currentTime.dateGreaterThan =
                                    currentTime["dateGreaterThan"].asString();
                        }
                        if (currentTime.isMember("dateGreaterThanEquals")) {
                            ac.condition.currentTime.dateGreaterThanEquals =
                                    currentTime["dateGreaterThanEquals"].asString();
                        }
                    }

                    //if (!condition["secureTransport"].isNull()) {
                    if (condition.isMember("secureTransport")) {
                        ac.condition.secureTransport = condition["secureTransport"].asBool();
                    }
                }
                // if (!access_control["effect"].isNull()) {
                if (access_control.isMember("effect")) {
                    ac.effect = access_control["effect"].asString();
                }
            }
            access_control_list.push_back(ac);
        }
    } catch (...) {
        LOGF(ERROR, "deserialize json to access_control_list exceptional!");
        return RET_ILLEGAL_OPERATION;
    }
    return RET_OK;
}

END_CPPSDK_NAMESPACE
