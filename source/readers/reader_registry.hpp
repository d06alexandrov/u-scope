#pragma once

#include <QCoreApplication>
#include <QString>
#include <QWidget>
#include <concepts>
#include <functional>
#include <memory>
#include <vector>

struct UniversalReaderDialogConfig;

/**
 * @brief Metadata of a reader module.
 */
struct ReaderModuleConfig
{
    QString id; /**< Unique ID of the reader module. */

    const char *label_source =
            nullptr; /**< Translation source string for the reader module's label. */
    const char *label_context =
            nullptr; /**< Translation context string for the reader module's label. */

    /**
     * @brief Open the reader's configuration dialog and block until it is closed.
     *
     * @param parent_window Parent widget for the dialog.
     * @return New config if the dialog was accepted, nullptr if it was cancelled.
     */
    std::function<std::shared_ptr<UniversalReaderDialogConfig>(QWidget *parent_window)> open_dialog;

    /**
     * @brief Get the translated label.
     *
     * @return Translated label of the reader module.
     */
    [[nodiscard]] QString label() const
    {
        return QCoreApplication::translate(label_context, label_source);
    }
};

/**
 * @brief Registry for reader modules.
 */
class ReaderModuleRegistry
{
public:
    /**
     * @brief Get the singleton instance of the registry.
     *
     * @return Reference to the singleton instance of ReaderModuleRegistry.
     */
    static ReaderModuleRegistry &instance()
    {
        static ReaderModuleRegistry registry;
        return registry;
    }

    /**
     * @brief Register a new reader module.
     *
     * @param config Configuration of the reader module to register.
     */
    void register_module(ReaderModuleConfig config) { m_modules.push_back(std::move(config)); }

    /**
     * @brief Get the list of registered reader modules.
     *
     * @return Vector of ReaderModuleConfig representing the registered reader modules.
     */
    [[nodiscard]] const std::vector<ReaderModuleConfig> &modules() const { return m_modules; }

private:
    std::vector<ReaderModuleConfig> m_modules; /**< List of registered reader modules. */
};

/**
 * @brief Concept for reader modules.
 */
template <typename T>
concept ReaderModule = requires(QWidget *parent_window) {
    { T::module_id() } -> std::convertible_to<QString>;
    { T::label_source() } -> std::convertible_to<const char *>;
    { T::label_context() } -> std::convertible_to<const char *>;
    { T::open_dialog(parent_window) } -> std::same_as<std::shared_ptr<UniversalReaderDialogConfig>>;
};

/**
 * @brief Registrar for reader modules.
 *
 * This template struct is used to automatically register a reader module with the
 * ReaderModuleRegistry. It requires the reader module to satisfy the ReaderModule concept.
 */
template <ReaderModule T>
struct ReaderModuleRegistrar
{
    ReaderModuleRegistrar()
    {
        ReaderModuleRegistry::instance().register_module(ReaderModuleConfig{
                .id = T::module_id(),
                .label_source = T::label_source(),
                .label_context = T::label_context(),
                .open_dialog = &T::open_dialog,
        });
    }
};
