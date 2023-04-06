from FMT import Core
from FMT import Models
from FMT import Parser

if __name__ == "__main__":
        attributes = [
		["GS1","GS2","GS3"],
		["FC4","FC6"],
		["COS8","COS20","COS30","COS60"]]
        nomsattributs= [["GS blabla 1","GS blabla 2","GS blabla 3"],
                        ["Courbe 1","Courbe 2"],
                        ["COS blabla 1","COS blabla 2","COS blabla 3","COS blabla 4"]]
        aggregates = [[""],[""],["COSSTA"]]
        elementsaggregats = [[[]],[[]],[["COS8","COS20"]]]
        themes = []
        fullsize = 0
        themeid = 0
        for attribute,nomsattribut,aggregate,elementsaggregat in zip(attributes,nomsattributs,aggregates,elementsaggregats):
                themes.append(Core.FMTtheme(attribute,nomsattribut,aggregate,elementsaggregat,themeid,fullsize,"Nom du theme"))
                themeid+=1
                fullsize += len(attribute)
        area = [
                Core.FMTactualdevelopment(Core.FMTmask("GS1 FC6 COS30", themes),5,0,1200.00),
                Core.FMTactualdevelopment(Core.FMTmask("GS2 FC4 COS20", themes),10,0,1456.00),
                Core.FMTactualdevelopment(Core.FMTmask("GS3 FC6 COS60", themes),20,1,2352.00),
                Core.FMTactualdevelopment(Core.FMTmask("GS1 FC4 COS30", themes),25,0,5454.00)
                ]
        yields=Core.FMTyields()
        fc6yield=Core.FMTageyieldhandler(Core.FMTmask("? FC6 ?",themes))
        fc4yield=Core.FMTageyieldhandler(Core.FMTmask("? FC4 ?",themes))
        fc6j = 0
        fc4j = 0
        for age in range(1,51):
                fc6yield.push_base(age)
                fc4yield.push_base(age)
                fc6yield.push_data("YVTOT", fc6j)
                fc4yield.push_data("YVTOT", fc4j)
                fc6j += 10
                fc4j += 5
        yields.push_back(Core.FMTmask("? FC6 ?",themes),fc6yield)
        yields.push_back(Core.FMTmask("? FC4 ?",themes),fc4yield)
        yields.update()
        actions = [
                Core.FMTaction("ACT",True,True)
                ]
        actionspec = Core.FMTspec()
        actionspec.addbounds(Core.FMTagebounds(Core.FMTsection.Action,100,3))
        actions[0].push_back(Core.FMTmask("? ? ?",themes),actionspec)
        actions[0].update()
        transitions = [Core.FMTtransition("ACT")]
        fork = Core.FMTfork()
        fork.add(Core.FMTtransitionmask("? ? ?", themes, 100.0))
        transitions[0].push_back(Core.FMTmask("? ? ?", themes),fork)
        transitions[0].update()
        lifespan = Core.FMTlifespans()
        lifespan.push_back(Core.FMTmask("? ? ?", themes),80)
        lifespan.update()
        sources = [Core.FMToutputsource(Core.FMTspec(),Core.FMTmask("? ? ?", themes),Core.FMTotar.actual, "YVTOT", "ACT", 0, -1)]
        outputs = [Core.FMToutput("OVOLTOTREC","VOLUME RECOLTE","VOLUME",sources,[])]
        objective = Core.FMTconstraint(Core.FMTconstrainttype.FMTMAXMINobjective,outputs[0])
        objective.setlength(1,30)
        evenflow = Core.FMTconstraint(Core.FMTconstrainttype.FMTevenflow, outputs[0])
        evenflow.setlength(1,30)
        constraints = [objective,evenflow]
        model = Models.FMTlpmodel(Models.FMTmodel(),Models.FMTsolverinterface.CLP)
        model.setname("TEST")
        model.setthemes(themes)
        model.setyields(yields)
        model.setarea(area)
        model.setlifespan(lifespan)
        model.settransitions(transitions)
        model.setactions(actions)
        model.setoutputs(outputs)
        model.setconstraints(constraints)
        model.setparameter(Models.FMTintmodelparameters.LENGTH, 10)
        mparser = Parser.FMTmodelparser() 
        mparser.write(model, "../../tests/createmodel/")
        model.doplanning(True)
        for output in outputs:
                for period in range(0,11):
                        print(output.getname()+" of value ",model.getoutput(output,period,Core.FMToutputlevel.totalonly)," at period ",period)


