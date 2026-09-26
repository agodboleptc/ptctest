# Diagram Index

Every diagram in this documentation set was authored as an inline Mermaid block
inside its narrative doc, so it renders next to the explanation that justifies it.
The files here are **verbatim extractions** of those same blocks, provided as
standalone `.mmd` files for tooling that wants to render/version diagrams
separately from prose. The narrative doc is the authoritative source in case of any
drift between the two — if you edit a diagram, edit it in the source `.md` file
first and re-extract.

| File | Diagram | Source doc |
|---|---|---|
| `02_major_components.mmd` | Major Components (all 4 modules + external deps) | `docs/02_architecture_overview.md` §2 |
| `02_module_dependencies.mmd` | Module-level dependency direction | `docs/02_architecture_overview.md` §4 |
| `02_ownership.mmd` | Object ownership graph (pimSessionInfo → pimEntitlement → 7 wrapper/Loop pairs) | `docs/02_architecture_overview.md` §5 |
| `03_pimInstallerRun_sequence.mmd` | Full `pimInstallerRun` sequence, init through shutdown | `docs/03_application_startup.md` §3 |
| `04_onexecute_dispatch.mmd` | `pimEntitlement::OnExecute` dispatch flowchart | `docs/04_installation_flow.md` §1 |
| `04_oninstall_pipeline.mmd` | `pimEntitlement::OnInstall` stage-by-stage pipeline | `docs/04_installation_flow.md` §2 |
| `06_entitlement_lifecycle.mmd` | Entitlement lifecycle state diagram | `docs/06_entitlement_framework.md` §2 |
| `08_xml_document_relationships.mmd` | Cross-document XML relationships (product/image/session/URL/translation/web-service) | `docs/08_xml_configuration.md` §8 |

All 8 diagrams use the confirmed source-evidence conventions of the rest of this
documentation set — dashed/labeled edges noting `inferred`/`UNKNOWN` items are
preserved verbatim from the originating doc.
