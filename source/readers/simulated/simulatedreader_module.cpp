#include "reader_registry.hpp"
#include "simulatedreader_dialog_model.hpp"

namespace {

struct SimulatedReaderModule
{
    static QString module_id() { return QStringLiteral("simulated"); }
    static const char *label_source() { return QT_TR_NOOP("Simulated source"); }
    static const char *label_context() { return "SourceListController"; }
    static QUrl dialog_url()
    {
        return { QStringLiteral("qrc:/qt/qml/UI/Readers/Simulated/qml/SimulatedReaderDialog.qml") };
    }

    static std::shared_ptr<UniversalReaderDialogConfig> build_config(QObject *session_model)
    {
        auto *model = qobject_cast<SimulatedReaderDialogModel *>(session_model);
        return model ? model->build_config() : nullptr;
    }
};

const ReaderModuleRegistrar<SimulatedReaderModule> simulated_reader_registrar;

} // namespace
