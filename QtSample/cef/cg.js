"use strict";

const fs = require("fs");

global.callNames = {};
global.processedCallNames = [];

const IgnoreNames = [
    "objectName",
    "deleteLater",
    "_q_reregisterTimers",
    "destroyed",
    "objectNameChanged",
];

const EnumTypes = [
    "MyEnumType",
];

const [pType, pRetType, pNames, pArgTypes] = [ 0, 1, 2, 3 ];

function castCppEnum(type) {
    return (EnumTypes.indexOf(type) === -1)
         ? type
         : "::enum::";
}

function getClassName(names) {
    return names.slice(0, names.length - 1).join("::");
}

function getCallName(entry) {
    const type = entry[pType];
    if (type !== "Slot") {
        throw new Error(`Wrong type ${type}`);
    }
    const names = entry[pNames];
    const className = getClassName(names);
    return global.callNames[className]
         ? global.callNames[className] + "." + names[names.length - 1]
         : names[names.length - 1];
}

function toString(entry, i, slotIndex) {
    return `
    const auto arg${i}_before_cast = argsArray.at(${i});
    QString arg${i};
    if (arg${i}_before_cast.isString()) {
        arg${i} = arg${i}_before_cast.toString();
    } else if (arg${i}_before_cast.isBool()) {
        // explicit cast here, some of the interfaces rely on implicit casts. 
        arg${i} = arg${i}_before_cast.toBool() ? "true" : "false";
    } else {
        attempts.push_back("Cannot cast arg${i} to QString for ${entry[pNames].join("::")}.");
        goto DONE_${slotIndex};
    }`;
}

function toVariantMap(entry, i, slotIndex) {
    return `
    const auto arg${i}_before_cast = argsArray.at(${i});
    QVariantMap arg${i};
    if (arg${i}_before_cast.isObject()) {
        arg${i} = arg${i}_before_cast.toObject().toVariantMap();
    } else if (arg${i}_before_cast.isNull() ||
               arg${i}_before_cast.isUndefined()) {
        // empty object 
    } else {
        attempts.push_back("Cannot cast arg${i} to QVariantMap for ${entry[pNames].join("::")}.");
        goto DONE_${slotIndex};
    }`;
}

function toNumber(entry, i, slotIndex) {
    return `
    const auto arg${i}_before_cast = argsArray.at(${i});
    double arg${i};
    if (arg${i}_before_cast.isDouble()) {
        arg${i} = arg${i}_before_cast.toDouble();                
    } else if (arg${i}_before_cast.isString()) {
        // Fix signature
        QTextStream ts;
        ts << arg${i}_before_cast.toString();
        ts >> arg${i};
    } else {
        attempts.push_back("Cannot cast arg${i} to number for ${entry[pNames].join("::")}.");
        goto DONE_${slotIndex};
    }`;
}

function toEnum(entry, i, slotIndex) {
    return `
    const auto arg${i}_before_cast = argsArray.at(${i});
    ${entry[pArgTypes][i]} arg${i};
    if (arg${i}_before_cast.isDouble()) {
        arg${i} = ${entry[pArgTypes][i]}(int(arg${i}_before_cast.toDouble()));                
    } else {
        attempts.push_back("Cannot cast arg${i} to enum for ${entry[pNames].join("::")}.");
        goto DONE_${slotIndex};
    }`;
}

function toBool(entry, i, slotIndex) {
    return `
    const auto arg${i}_before_cast = argsArray.at(${i});
    if (!arg${i}_before_cast.isBool()) {
        attempts.push_back("Cannot cast arg${i} to bool for ${entry[pNames].join("::")}.");
        goto DONE_${slotIndex};
    }
    const auto arg${i} = arg${i}_before_cast.toBool();`;
}

function toStringList(entry, i, slotIndex) {
    return `
    const auto arg${i}_before_cast = argsArray.at(${i});
    if (!arg${i}_before_cast.isArray()) {
        attempts.push_back("Cannot cast arg${i} to QStringList for ${entry[pNames].join("::")}.");
        goto DONE_${slotIndex};
    }
    const auto arg${i}_half_cast = arg${i}_before_cast.toArray().toVariantList();
    QStringList arg${i};
    for (auto i = arg${i}_half_cast.cbegin(); i != arg${i}_half_cast.cend(); i++) {
        if (i->canConvert(QMetaType::QString)) {
            const auto str = i->toString();
            arg${i} << str;
        } else {
            attempts.push_back("Cannot cast arg${i} to QStringList for ${entry[pNames].join("::")}.");
            goto DONE_${slotIndex};
        }
    }`;
}

function toVariantList(entry, i, slotIndex) {
    return `
    const auto arg${i}_before_cast = argsArray.at(${i});
    if (!arg${i}_before_cast.isArray()) {
        attempts.push_back("Cannot cast arg${i} to QVariantList for ${entry[pNames].join("::")}.");
        goto DONE_${slotIndex};
    }

    const auto arg${i} = arg${i}_before_cast.toArray().toVariantList();`
}

function toVariant(entry, i) {
    return `
    const auto arg${i}_before_cast = argsArray.at(${i});
    const auto arg${i} = arg${i}_before_cast.toVariant();`
}

function toQtArgs(entry, slotIndex) {
    // on CEF side, arguments are passed in as "const CefV8ValueList& arguments",
    // here we need to convert them into corresponding Qt data types.
    const args = entry[pArgTypes];
    const buffer = [];

    buffer.push(`if (${args.length} != argsArray.count()) {
        attempts.push_back("Arguments count mismatch(${entry[pNames].join("::")}), expecting ${args.length}.");
        goto DONE_${slotIndex};
    }`);
    for (let i = 0; i < args.length; i++) {
        const arg = castCppEnum(args[i]);
        switch (arg) {
            case "QString": {
                buffer.push(toString(entry, i, slotIndex));
                break;
            }
            case "QVariantMap": {
                buffer.push(toVariantMap(entry, i, slotIndex));
                break;
            }
            case "int":
            case "uint":
            case "float":
            case "double":
            case "long":
            case "ulong":
            case "qlonglong": {
                buffer.push(toNumber(entry, i, slotIndex));
                break;
            }
            case "::enum::": {
                buffer.push(toEnum(entry, i, slotIndex));
                break;
            }
            case "bool": {
                buffer.push(toBool(entry, i, slotIndex));
                break;
            }
            case "QStringList": {
                buffer.push(toStringList(entry, i, slotIndex));
                break;
            }
            case "QVariantList": {
                buffer.push(toVariantList(entry, i, slotIndex));
                break;
            }
            case "QVariant": {
                buffer.push(toVariant(entry, i, slotIndex));
                break;
            }
            default: {
                throw new Error(`Unsupported DataType: "${arg}"`);
            }
        }
    }
    return buffer.join("\n");
}

function fillArgs(entry) {
    const len = entry[pArgTypes].length;
    return Array.from(new Array(len), (x, i) => `arg${i}`);
}

function processSlot(entry, slotIndex) {
    const type = entry[pType];
    if (type !== "Slot") {
        throw new Error(`Wrong type ${type}`);
    }
    const callName = getCallName(entry);
    global.processedCallNames.push(callName);

    return `if (methodName == "${callName}") {
    ${toQtArgs(entry, slotIndex)}
    // TODO: Use Try Block?
    ${entry[pRetType] === "void" ? "" : "const auto result = "}bridge->${callName.replace(/\./g, "->")}(${fillArgs(entry).join(", ")});

    QJsonArray argsArray;
    ${entry[pRetType] === "void" ? "" : "argsArray << QJsonValue::fromVariant(result);"}
   
    // To keep the string version of the JSON value escaped
    // here to use the functionality provided by QDebug
    QString tmpStr;
    QDebug(&tmpStr) << QString(QJsonDocument(argsArray).toJson(QJsonDocument::Compact)); // Have to cast to QString first, otherwise mojibake
    
    const auto valueStr = tmpStr.toStdString();

    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("CallReturn");
    CefRefPtr<CefListValue> args = msg->GetArgumentList();

    args->SetInt(0, callId);
    args->SetString(1, valueStr);

    browser->SendProcessMessage(PID_RENDERER, msg);

    return true;
}
DONE_${slotIndex}:
`;
}

function getJsSignalName(entry) {
    const names = entry[pNames];
    const className = getClassName(names);
    if (className in global.callNames) {
        return `${global.callNames[className]}.${names[names.length - 1]}`;
    } else {
        return names[names.length - 1];
    }
}

function toJsonArgs(entry) {
    const types = entry[pArgTypes];
    return types.map((type, i) => {
        type = castCppEnum(type);
        switch (type) {
            case "QString":
            case "int":
            case "bool":
            case "double":
            case "::enum::": {
                return `const auto& arg${i} = arg${i}_original;`;
            }
            case "QVariantMap": {
                return `QJsonObject arg${i} = QJsonObject::fromVariantMap(arg${i}_original);`;
            }
            case "QVariantList": {
                return `QJsonArray arg${i} = QJsonArray::fromVariantList(arg${i}_original);`;
            }
            default: {
                throw new Error(`Unsupported data type: ${type}`);
            }
        }
    }).join("\n    ");
}

function processSignal(entry) {
    const type = entry[pType];
    if (type !== "Signal") {
        throw new Error(`Wrong type ${type}`);
    }

    const types = entry[pArgTypes];
    const numberedTypes = types.map((type, i) => {
        type = castCppEnum(type);
        return `${type} arg${i}_original`;
    }).join(", ");

    const className = entry[pNames].slice(0, entry[pNames].length - 1).join("::");
    const targetName = className in global.callNames
        ? "bridge->" + global.callNames[className]
        : "bridge";
    return `QObject::connect(${targetName}, &${entry[pNames].join("::")}, [bridge, browser](${numberedTypes}) {
    QJsonArray argsArray;
    ${toJsonArgs(entry)}
    ${entry[pArgTypes].length ? `argsArray << ${fillArgs(entry).join(" << ")};` : ""}
    
    // To keep the string version of the JSON value escaped
    // here to use the functionality provided by QDebug
    QString tmpStr;
    QDebug(&tmpStr) << QString(QJsonDocument(argsArray).toJson(QJsonDocument::Compact)); // Have to cast to QString first, otherwise mojibake
    
    const auto valueStr = tmpStr.toStdString();

    CefRefPtr<CefProcessMessage> msg = CefProcessMessage::Create("SignalEmit");
    CefRefPtr<CefListValue> args = msg->GetArgumentList();

    args->SetString(0, "${getJsSignalName(entry)}");
    args->SetString(1, valueStr);

    browser->SendProcessMessage(PID_RENDERER, msg);
});`;
}

function processEntries(entries) {
    const clientSlotHandlerBuffer = [];
    const clientSignalHandlerBuffer = [];

    for (let entryIndex = 0; entryIndex < entries.length; entryIndex++) {
        const entry = entries[entryIndex];
        switch (entry[pType]) {
            case "Slot": {
                clientSlotHandlerBuffer.push(processSlot(entry, entryIndex));
                break;
            }
            case "Object": {
                // buffer.push(`/* NOT IMPLEMENTED: ${entry} */`);
                break;
            }
            case "Signal": {
                clientSignalHandlerBuffer.push(processSignal(entry));
                break;
            }
            default: {
                throw new Error(`Unexpected in for entry of entries: ${entry}`);
            }
        }
    }
    fs.writeFileSync("./cg/ClientSlot.cppf", clientSlotHandlerBuffer.join("\n"));
    fs.writeFileSync("./cg/ClientSignal.cppf", clientSignalHandlerBuffer.join("\n"));
}

function shouldIgnore (entry) {
    const methodNameItem = entry[pNames];
    const methodName = methodNameItem[methodNameItem.length - 1];
    return IgnoreNames.indexOf(methodName) !== -1;
}

function main() {
    try {
        fs.mkdirSync("cg");
    } catch(e) {
        if (e.code !== "EEXIST") {
            throw e;
        }
    }
    const lines = fs.readFileSync("./cg/Bridge.def", {
        encoding: "UTF-8",
    }).split("\n");

    const entries = [];
    for (let line of lines) {
        const parts = line.replace(/\t/g, " ").split(" ");
        if (parts.length === 1 && parts[0] === "") {
            continue;
        }
        const entry = parts.filter(i => i !== "");
        if (entry.length !== 3) {
            throw new Error(`Invalid line parse: ${line}`);
        }

        if (entry[pType] === "Property") {
            entry[pNames] = entry[pNames].split("::");
        }

        if (entry[pType] === "Signal" ||
            entry[pType] === "Slot") {
            const tmp = entry[2];
            const pos = tmp.indexOf("(");
            entry[pNames] = tmp.substr(0, pos).split("::");

            const args = tmp.substring(pos + 1, tmp.length - 1).split(",");
            entry.push(args.filter(arg => arg !== ""));
        }

        if (entry[pType] === "Object") {
            const names = entry[pNames];
            const tmp = names.split("::");
            const callPath = tmp[tmp.length - 1];

            const className = entry[pRetType].substr(0, entry[pRetType].length - 1);
            if (className in global.callNames) {
                throw new Error(`Duplicate ClassName Found: ${entry}`);
            }
            global.callNames[className] = callPath;
        }

        if (shouldIgnore(entry)) {
            continue;
        }

        entries.push(entry);
    }
    processEntries(entries);

    const exportedSymbols = Array.from(new Set(global.processedCallNames));

    const exports = `#ifndef CODE_GEN_BRIDGE_EXPORTS_H
#define CODE_GEN_BRIDGE_EXPORTS_H
static std::list<std::string> EXPORTED_FUNCTIONS = {
${exportedSymbols.map(n => `  "${n}"`).join(",\n")}
};
#endif // CODE_GEN_BRIDGE_EXPORTS_H
`;
    fs.writeFileSync("./cg/exports.h", exports);

}

main();
