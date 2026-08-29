#include "reader_registry.hpp"
#include "serialreader_dialog.h"

namespace {

struct SerialReaderModule
{
    static QString module_id() { return QStringLiteral("serial"); }
    static const char *label_source() { return QT_TR_NOOP("Serial source"); }
    static const char *label_context() { return "SerialReaderModule"; }

    static std::shared_ptr<UniversalReaderDialogConfig> open_dialog(QWidget *parent_window)
    {
        SerialReaderDialog dialog(parent_window);

        if (dialog.exec() == QDialog::Accepted) {
            return dialog.get_config();
        }

        return nullptr;
    }
};

const ReaderModuleRegistrar<SerialReaderModule> serial_reader_registrar;

} // namespace
