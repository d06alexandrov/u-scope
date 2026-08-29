#include "reader_registry.hpp"
#include "simulatedreader_dialog.h"

namespace {

struct SimulatedReaderModule
{
    static QString module_id() { return QStringLiteral("simulated"); }
    static const char *label_source() { return QT_TR_NOOP("Simulated source"); }
    static const char *label_context() { return "SimulatedReaderModule"; }

    static std::shared_ptr<UniversalReaderDialogConfig> open_dialog(QWidget *parent_window)
    {
        SimulatedReaderDialog dialog(parent_window);

        if (dialog.exec() == QDialog::Accepted) {
            return dialog.get_config();
        }

        return nullptr;
    }
};

const ReaderModuleRegistrar<SimulatedReaderModule> simulated_reader_registrar;

} // namespace
