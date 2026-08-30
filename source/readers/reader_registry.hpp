#pragma once

#include <QCoreApplication>
#include <QString>
#include <QUrl>
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

    QUrl dialog_url; /**< QML URL of the configuration dialog for this reader. */

    /**
     * @brief Build a reader configuration from the dialog's QML-side session model.
     *
     * @param session_model Backing QObject the dialog exposes to QML, filled in by the user.
     * @return New reader configudarion.
     */
    std::function<std::shared_ptr<UniversalReaderDialogConfig>(QObject *session_model)>
            build_config;

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
concept ReaderModule = requires(QObject *session_model) {
    { T::module_id() } -> std::convertible_to<QString>;
    { T::label_source() } -> std::convertible_to<const char *>;
    { T::label_context() } -> std::convertible_to<const char *>;
    { T::dialog_url() } -> std::convertible_to<QUrl>;
    {
        T::build_config(session_model)
    } -> std::same_as<std::shared_ptr<UniversalReaderDialogConfig>>;
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
                .dialog_url = T::dialog_url(),
                .build_config = &T::build_config,
        });
    }
};
