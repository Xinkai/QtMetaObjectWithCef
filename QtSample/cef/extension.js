"use strict";

const signalMap = Object.create(null);
const promiseMap = new Map();

function makeCallable(names) {
    const result = new Proxy(Object.create(null), {
        get(obj, prop, receiver) {
            const newNames = [...names, prop];
            return makeCallable(newNames);
        },
        apply(obj, that, args) {
            const func = window[names.join(".")];
            if (!func) {
                const funcName = names[names.length - 1];
                const cb = args[0];
                if (typeof cb === "function") {
                    const eventName = names.slice(0, names.length - 1).join(".");
                    if (funcName === "connect") {
                        if (!signalMap[eventName]) {
                            signalMap[eventName] = [];
                        }
                        signalMap[eventName].push(cb);
                        return;
                    }
                    if (funcName === "disconnect") {
                        if (!signalMap[eventName]) {
                            return;
                        }
                        signalMap[eventName] = signalMap[eventName].map(one => one != cb);
                        return;
                    }
                }

                return Promise.reject(`Cannot find function ${names.join(".")}`);
            }
            
            let jsonArgs;
            try {
                jsonArgs = JSON.stringify(args);
            } catch (e) {
                return Promise.reject(`Cannot stringify with ${args}`);
            }
            
            const callId = func(jsonArgs);
            return new Promise((resolve, reject) => {
                promiseMap.set(callId, [resolve, reject]);
            });
        },
    });
    return result;
}

const bridge = makeCallable([]);

function onCallReturn(callId, payloadStr) {
    const [resolve, reject] = promiseMap.get(callId);
    promiseMap.delete(callId);

    if (resolve) {
        let payload;
        try {
            payload = JSON.parse(payloadStr);
        } catch (e) {
            console.error(`onCallReturn(${callId}) JSON parse failed`);
            throw e;
        }
        resolve(...payload);
    }
}

function onCallFailed(callId, reason) {
    const [resolve, reject] = promiseMap.get(callId);
    promiseMap.delete(callId);

    if (reject) {
        reject(reason);
    }
}

function onSignalEmit(signalName, payloadStr) {
    const callbacks = signalMap[signalName];
    if (callbacks) {
        let payload;
        try {
            payload = JSON.parse(payloadStr);
        } catch (e) {
            console.error(`onSignalEmit(${signalName}) JSON parse failed`);
            throw e;
        }
        for (let callback of callbacks) {
            try {
                callback(...payload);
            } catch (e) {
                console.error(`onSignalEmit(${signalName}) callback(${callback.name}) runtime error`);
                throw e;
            }
        }
    }
}
