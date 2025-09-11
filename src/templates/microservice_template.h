#pragma once
#include "template_base.h"
#include <unordered_map>
#include <memory>

namespace templates {

// Microservice configuration options
struct MicroserviceOptions {
    std::string serviceName;
    std::string serviceVersion = "1.0.0";
    std::string apiVersion = "v1";
    int defaultPort = 8080;
    std::string protocol = "http"; // http, grpc, both
    bool enableMetrics = true;
    bool enableHealthCheck = true;
    bool enableLogging = true;
    bool enableTracing = false;
    bool enableAuthentication = false;
    bool enableRateLimiting = false;
    bool enableServiceDiscovery = false;
    bool enableConfigService = false;
    std::vector<std::string> dependencies; // Other services this depends on
    std::string databaseType = "none"; // postgresql, mysql, mongodb, redis, none
    std::string messageQueue = "none"; // rabbitmq, kafka, redis, none
    std::string containerRuntime = "docker"; // docker, podman
    std::string orchestration = "none"; // kubernetes, docker-compose, none
};

class MicroserviceTemplate : public TemplateBase {
public:
    explicit MicroserviceTemplate(const ProjectOptions& options);
    virtual ~MicroserviceTemplate() = default;

    bool generateProject() override;

private:
    MicroserviceOptions microserviceOptions_;

    // Core generation methods
    bool generateServiceStructure();
    bool generateApiLayer();
    bool generateBusinessLogic();
    bool generateDataLayer();
    bool generateConfiguration();
    bool generateDocumentation();
    bool generateDeployment();
    bool generateTests();

    // API layer generation
    bool generateRestApi();
    bool generateGrpcApi();
    bool generateApiModels();
    bool generateApiControllers();
    bool generateApiMiddleware();
    bool generateOpenApiSpec();

    // Business logic generation
    bool generateServices();
    bool generateRepositories();
    bool generateDomainModels();
    bool generateValidators();
    bool generateEventHandlers();

    // Data layer generation
    bool generateDatabaseModels();
    bool generateMigrations();
    bool generateDatabaseConfig();
    bool generateConnectionPool();

    // Infrastructure generation
    bool generateLogging();
    bool generateMetrics();
    bool generateHealthCheck();
    bool generateTracing();
    bool generateSecurity();
    bool generateRateLimiting();

    // Deployment generation
    bool generateDockerfile();
    bool generateDockerCompose();
    bool generateKubernetesManifests();
    bool generateHelmChart();
    bool generateCI();

    // Configuration methods
    void configureMicroserviceOptions();
    std::string generateServiceMain();
    std::string generateServiceConfig();
    std::string generateServiceInterface();

    // CMake generation for microservices
    std::string generateMicroserviceCMakeLists();
    std::string generateServiceCMakeLists();
    std::string generateTestCMakeLists();

    // Docker and containerization
    std::string generateDockerfileContent();
    std::string generateDockerComposeContent();
    std::string generateDockerIgnore();

    // Kubernetes manifests
    std::string generateDeploymentYaml();
    std::string generateServiceYaml();
    std::string generateConfigMapYaml();
    std::string generateIngressYaml();

    // Monitoring and observability
    std::string generatePrometheusConfig();
    std::string generateGrafanaDashboard();
    std::string generateJaegerConfig();

    // Database specific generation
    bool generatePostgreSQLIntegration();
    bool generateMongoDBIntegration();
    bool generateRedisIntegration();

    // Message queue integration
    bool generateRabbitMQIntegration();
    bool generateKafkaIntegration();

    // Security components
    std::string generateJWTHandler();
    std::string generateOAuthConfig();
    std::string generateSecurityMiddleware();

    // Service mesh integration
    std::string generateIstioConfig();
    std::string generateConsulConfig();

    // Testing utilities
    std::string generateIntegrationTests();
    std::string generateLoadTests();
    std::string generateContractTests();
    std::string generateTestContainers();

    // Utility methods
    std::string formatServiceName(const std::string& name);
    std::string generateServiceId();
    std::unordered_map<std::string, std::string> getTemplateVariables();
    bool validateMicroserviceConfiguration();
};

} // namespace templates
