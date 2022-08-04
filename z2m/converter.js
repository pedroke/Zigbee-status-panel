/* eslint-disable */
const zigbeeHerdsmanConverters = require('zigbee-herdsman-converters');
const exposes = require('zigbee-herdsman-converters/lib/exposes');
const e = exposes.presets;
const ea = exposes.access;

const fz = {
      ZigSP_parse: {
      cluster: 'genBasic',
      type: ['attributeReport', 'readResponse'],
      convert: (model, msg, publish, options, meta) => {
            return {
                insideTemperature: msg.data['41364'],
                outsideTemperature: msg.data['41365'],
                warnings: msg.data['41366'],
            };
        },
      },
};

const tz = {
        ZigSP_insideTemperature: {
        key: ['insideTemperature'],
        convertSet: async (entity, key, value, meta) => {
                const opts = {
                        timeout: 20000,
                };

                await entity.write('genBasic', {41364: {value: value, type: 0x29}}, opts);

        },
    },
        ZigSP_outsideTemperature: {
        key: ['outsideTemperature'],
        convertSet: async (entity, key, value, meta) => {
                const opts = {
                        timeout: 20000,
                };

                await entity.write('genBasic', {41365: {value: value, type: 0x29}}, opts);

        },
    },
        ZigSP_warnings: {
        key: ['warnings'],
        convertSet: async (entity, key, value, meta) => {
                const opts = {
                        timeout: 20000,
                };

                await entity.write('genBasic', {41366: {value: value, type: 0x20}}, opts);

        },
    },
};

const hassLinkquality = {
        type: 'sensor',
        object_id: 'linkquality',
        discovery_payload: {
                unit_of_measurement: 'lqi',
                value_template: '{{ value_json.linkquality }}',
        },
}

const devices = [
        {
        zigbeeModel: ['ZigSP'],
        model: 'ZigSP',
        vendor: 'Open smart home',
                description: '[Zigbee HA status panel](https://github.com/pedroke/ZigStatusPanel)',
        fromZigbee: [fz.ZigSP_parse],
        toZigbee: [tz.ZigSP_insideTemperature, tz.ZigSP_outsideTemperature, tz.ZigSP_warnings],
                meta: {
                configureKey: 1,
        },
        exposes: [exposes.numeric('insideTemperature', ea.SET).withValueMin(0).withValueMax(2000).withUnit('°C'), exposes.numeric('outsideTemperature', ea.SET).withValueMin(-400).withValueMax(50$
        configure: async (device, coordinatorEndpoint) => {
                // dummy
                },
                homeassistant: [hassLinkquality],
        },
];

