#include "reader_registry.hpp"
#include "serialreader_dialog_model.hpp"

namespace {

struct SerialReaderModule
{
    static QString module_id() { return QStringLiteral("serial"); }
    static const char *label_source() { return QT_TR_NOOP("Serial source"); }
    static const char *label_context() { return "SerialReaderModule"; }
    static QUrl dialog_url()
    {
        return { QStringLiteral("qrc:/qt/qml/UI/Readers/Serial/qml/SerialReaderDialog.qml") };
    }

    static std::shared_ptr<UniversalReaderDialogConfig> build_config(QObject *session_model)
    {
        auto *model = qobject_cast<SerialReaderDialogModel *>(session_model);
        return model ? model->build_config() : nullptr;
    }
};

const ReaderModuleRegistrar<SerialReaderModule> serial_reader_registrar;

} // namespace
