import QtQuick
import QtQuick.Controls

SpinBox {
    id: control

    property int decimals: 3
    readonly property int scaleFactor: Math.pow(10, decimals)
    readonly property real realValue: value / scaleFactor

    property real realFrom: 0
    property real realTo: 100

    function setRealValue(v) {
        value = Math.round(v * scaleFactor);
    }

    from: Math.round(realFrom * scaleFactor)
    to: Math.round(realTo * scaleFactor)

    validator: DoubleValidator {
        bottom: Math.min(control.realFrom, control.realTo)
        top: Math.max(control.realFrom, control.realTo)
        decimals: control.decimals
        notation: DoubleValidator.StandardNotation
    }

    textFromValue: (value, locale) => Number(value / control.scaleFactor).toLocaleString(locale, 'f', control.decimals)

    valueFromText: (text, locale) => Math.round(Number.fromLocaleString(locale, text) * control.scaleFactor)
}
