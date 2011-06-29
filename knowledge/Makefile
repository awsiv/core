
all: scan_texinfo scan_examples cf-QuickRef3.html scan_quickref3.cf scan_cf3ref.cf cf3-solutions.html scan_solutions.cf cf-copernicus.html scan_copernicus.cf cfnova.html scan_cfnova.cf examples.cf cf3-tutorial.html scan_tutorial3.cf unit_peers.html cf3-bestpractice.html scan_bp3.cf cf3-upgrade.html scan_upgrade.cf st-rollback.html scan_rollback.cf  st-adopt.html scan_adopt.cf  st-bdma.html scan_bdma.cf st-itil.html scan_itil.cf st-knowledge.html scan_knowledge.cf st-orchestrate.html scan_orchestrate.cf st-reporting.html scan_reporting.cf st-schedule.html scan_schedule.cf st-teams.html scan_teams.cf scan_stdlib.cf st-distsched.html scan_distsched.cf st-iterate.html scan_iterate.cf scan_glossary.cf cf3-glossary.html st-hierarchy.html st-appmgt.html scan_appl.cf scan_hierarchy.cf scan_virt.cf st-security.html scan_security.cf st-federation.html scan_federation.cf st-cdp.html scan_cdp.cf scan_cloud.cf st-cloud.html highlight_syntax st-opennebula.html scan_opennebula.cf st-scale.html scan_scale.cf st-rbac.html scan_rbac.cf st-change.html scan_change.cf st-menus.html scan_menus.cf st-monitoring.html scan_monitoring.cf st-editing.html scan_editing.cf

clean:
	rm scan*cf

scan_texinfo: scan_texinfo.c
	gcc -g -o scan_texinfo scan_texinfo.c

highlight_syntax: highlight_syntax.c
	gcc -g -o highlight_syntax $<

scan_examples: scan_examples.c
	gcc -g -o scan_examples scan_examples.c

cf3-glossary.html: ../Documentation/cf3-glossary.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_glossary.cf: cf3-glossary.html
	./scan_texinfo "special topic" $< > scan_glossary.cf

st-monitoring.html: ../Documentation/SpecialTopic_Monitoring.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_monitoring.cf: st-monitoring.html
	./scan_texinfo "special topic" $< > scan_monitoring.cf

st-editing.html: ../Documentation/SpecialTopic_Editing.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_editing.cf: st-editing.html
	./scan_texinfo "special topic" $< > scan_editing.cf

st-cdp.html: ../Documentation/SpecialTopic_ContentDrivenPolicies.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_cdp.cf: st-cdp.html
	./scan_texinfo "special topic" $< > scan_cdp.cf


st-change.html: ../Documentation/SpecialTopic_Change.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_change.cf: st-change.html
	./scan_texinfo "special topic" $< > scan_change.cf

st-cloud.html: ../Documentation/SpecialTopic_Cloud.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_cloud.cf: st-cloud.html
	./scan_texinfo "special topic" $< > scan_cloud.cf

st-menus.html: ../Documentation/SpecialTopic_MenuDrivenConfig.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_menus.cf: st-menus.html
	./scan_texinfo "special topic" $< > scan_menus.cf

st-scale.html: ../Documentation/SpecialTopic_Scalability.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_scale.cf: st-scale.html
	./scan_texinfo "special topic" $< > scan_scale.cf

st-rbac.html: ../Documentation/SpecialTopic_RBAC.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_rbac.cf: st-rbac.html
	./scan_texinfo "special topic" $< > scan_rbac.cf


st-opennebula.html: ../Documentation/SpecialTopic_OpenNebula.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_opennebula.cf: st-opennebula.html
	./scan_texinfo "special topic" $< > scan_opennebula.cf

st-federation.html: ../Documentation/SpecialTopic_Federation.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_federation.cf: st-federation.html
	./scan_texinfo "special topic" $< > scan_federation.cf

st-security.html: ../Documentation/SpecialTopic_Security.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_security.cf: st-security.html
	./scan_texinfo "special topic" $< > scan_security.cf

st-virt.html: ../Documentation/SpecialTopic_Virtualization.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_virt.cf: st-virt.html
	./scan_texinfo "special topic" $< > scan_virt.cf

st-hierarchy.html: ../Documentation/SpecialTopic_Hierarchy.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_hierarchy.cf: st-hierarchy.html
	./scan_texinfo "special topic" $< > scan_hierarchy.cf

st-appmgt.html: ../Documentation/SpecialTopic_ApplMgt.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_appl.cf: st-appmgt.html
	./scan_texinfo "special topic" $< > scan_appl.cf

st-rollback.html: ../Documentation/SpecialTopic_Rollback.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_rollback.cf: st-rollback.html
	./scan_texinfo "special topic" $< > scan_rollback.cf

st-adopt.html: ../Documentation/SpecialTopic_Adoption.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_adopt.cf: st-adopt.html
	./scan_texinfo "special topic" $< > scan_adopt.cf

st-bdma.html:  ../Documentation/SpecialTopic_BDMA.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_bdma.cf: st-bdma.html
	./scan_texinfo "special topic" $< > scan_bdma.cf

st-iterate.html:  ../Documentation/SpecialTopic_Iteration.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_iterate.cf: st-iterate.html
	./scan_texinfo "special topic" $< > scan_iterate.cf

st-itil.html: ../Documentation/SpecialTopic_ITIL.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_itil.cf: st-itil.html
	./scan_texinfo "special topic" $< > scan_itil.cf

st-distsched.html: ../Documentation/SpecialTopic_DistributedScheduling.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<

scan_distsched.cf: st-distsched.html
	./scan_texinfo "special topic" $< > scan_distsched.cf

st-knowledge.html: ../Documentation/SpecialTopic_Knowledge.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_knowledge.cf: st-knowledge.html
	./scan_texinfo "special topic" $< > scan_knowledge.cf

st-orchestragte.html: ../Documentation/SpecialTopic_Modules.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_orchestrate.cf: st-orchestrate.html
	./scan_texinfo "special topic" $< > scan_orchestrate.cf

st-reporting.html: ../Documentation/SpecialTopic_Reporting.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_reporting.cf: st-reporting.html
	./scan_texinfo "special topic" $< > scan_reporting.cf

st-schedule.html: ../Documentation/SpecialTopic_Schedule.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_schedule.cf: st-schedule.html
	./scan_texinfo "special topic" $< > scan_schedule.cf

st-teams.html: ../Documentation/SpecialTopic_Teamwork.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_teams.cf: st-teams.html
	./scan_texinfo "special topic" $< > scan_teams.cf

CfengineStdLibrary.html: stdlib.texinfo
	cp /home/mark/LapTop/CfengineProjects/CfengineCommunityOpenPromiseBodyLibrary/trunk/stdlib.texinfo .
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<

scan_stdlib.cf: CfengineStdLibrary.html
	./scan_texinfo "community library" CfengineStdLibrary.html > scan_stdlib.cf
	./highlight_syntax CfengineStdLibrary.html
	mv CfengineStdLibrary.trans CfengineStdLibrary.html

cf3-upgrade.html: ../Documentation/cf3-upgrade.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<

scan_upgrade.cf: cf3-upgrade.html
	./scan_texinfo "upgrading" cf3-upgrade.html > scan_upgrade.cf

cf3-bestpractice.html: ../Documentation/cf3-bestpractice.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
	./highlight_syntax cf3-bestpractice.html
	mv cf3-bestpractice.trans cf3-bestpractice.html

scan_bp3.cf: cf3-bestpractice.html
	./scan_texinfo "cf3 best practice" cf3-bestpractice.html > scan_bp3.cf

cf3-tutorial.html: ../Documentation/cf3-tutorial.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
	./highlight_syntax cf3-tutorial.html
	mv cf3-tutorial.trans cf3-tutorial.html

scan_tutorial3.cf: cf3-tutorial.html
	./scan_texinfo "cf3 tutorial" cf3-tutorial.html > scan_tutorial3.cf

cf3-solutions.html: ../Documentation/cf3-solutions.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
	./highlight_syntax cf3-solutions.html
	mv cf3-solutions.trans cf3-solutions.html

scan_solutions.cf: cf3-solutions.html
	./scan_texinfo "cf3 reference" cf3-solutions.html > scan_solutions.cf

cf3-reference.html: ../Cfengine3Reference/cf3-Reference.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
	./highlight_syntax cf3-reference.html
	mv cf3-reference.trans cf3-reference.html

scan_cf3ref.cf: cf3-reference.html
	./scan_texinfo "cf3 reference" cf3-reference.html > scan_cf3ref.cf

unit_peers.html: examples.cf

examples.cf: Units
	./scan_examples | grep warning

cf-QuickRef3.html:  ../Documentation/cf_Quickref3.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_quickref3.cf:  cf-QuickRef3.html
	./scan_texinfo "cf3 reference" cf-QuickRef3.html > scan_quickref3.cf


cf-copernicus.html:  ../Documentation/cf-copernicus.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<
scan_copernicus.cf:  cf-copernicus.html
	./scan_texinfo "Hints" cf-copernicus.html > scan_copernicus.cf

cfnova.html:  ../Documentation/cfnova.texinfo
	makeinfo  --html --no-split --css-include=cfcomdoc.css $<

scan_cfnova.cf: cfnova.html
	./scan_texinfo "cf3 reference" cfnova.html > scan_cfnova.cf


update:
	scp -P222 *.html root@cf001lin.cfengine.com:/iu/eternity/cfsite/app/documents/cf-manuals/
	scp -P222 ontology.html root@cf001lin.cfengine.com:/iu/eternity/cfsite/app/documents/cf-manuals/cf-ontology.html

	(cd graphs; scp -P222 * root@cf001lin.cfengine.com:/iu/eternity/cfsite/app/webroot/copernicus/graphs)
	scp -P222 *.png root@cf001lin.cfengine.com:/iu/eternity/cfsite/app/webroot/img/outside

