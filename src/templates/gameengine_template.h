#pragma once
#include "template_base.h"

class GameEngineTemplate : public TemplateBase {
public:
  GameEngineTemplate(const CliOptions &options);

  bool create() override;

protected:
  bool createProjectStructure() override;
  bool createBuildSystem() override;
  bool setupPackageManager() override;
  bool setupTestFramework() override;
  bool setupGraphicsFramework();
  bool setupAudioSystem();
  bool setupInputSystem();
  bool setupResourceManager();
  bool setupSceneSystem();
  bool setupPhysicsEngine();
  bool setupScriptingEngine();
  bool setupAssetPipeline();

private:
  // Game engine specific content generators
  std::string getEngineHeaderContent();
  std::string getEngineSourceContent();
  std::string getGameHeaderContent();
  std::string getGameSourceContent();
  std::string getRendererHeaderContent();
  std::string getRendererSourceContent();
  std::string getInputManagerHeaderContent();
  std::string getInputManagerSourceContent();
  std::string getResourceManagerHeaderContent();
  std::string getResourceManagerSourceContent();
  std::string getSceneManagerHeaderContent();
  std::string getSceneManagerSourceContent();
  std::string getEntityHeaderContent();
  std::string getEntitySourceContent();
  std::string getComponentHeaderContent();
  std::string getSystemHeaderContent();
  std::string getShaderContent(const std::string& shaderType);
  std::string getConfigContent();
  std::string getAssetsReadmeContent();
  std::string getGameEngineReadmeContent();

  // Build system content for game engine
  std::string getGameEngineCMakeContent();
  std::string getGameEngineMesonContent();
  std::string getGameEngineBazelContent();

  // Package manager content for game engine
  std::string getGameEngineVcpkgContent();
  std::string getGameEngineConanContent();

  // Documentation content
  std::string getGameEngineDoxygenContent();
  std::string getArchitectureDocContent();
  std::string getGettingStartedContent();

  // Asset pipeline
  std::string getAssetPipelineScriptContent();
  std::string getShaderCompilerScriptContent();

  // Example game content
  std::string getExampleGameContent();
  std::string getExampleSceneContent();

  // Configuration and settings
  std::string getEngineConfigContent();
  std::string getGraphicsConfigContent();
  std::string getAudioConfigContent();
  std::string getInputConfigContent();
};
