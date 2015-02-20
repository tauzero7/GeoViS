// ---------------------------------------------------------------------
//  Copyright (c) 2013-2014, Universitaet Stuttgart, VISUS, Thomas Mueller
//
//  This file is part of GeoViS.
//
//  GeoViS is free software: you can redistribute it and/or modify it
//  under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  GeoViS is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with GeoViS.  If not, see <http://www.gnu.org/licenses/>.
// ---------------------------------------------------------------------

#include "Obj/GvsBase.h"
#include "Parser/parse_metric.h"
#include "Parser/parse_helper.h"
#include "Parser/GvsParseScheme.h"

#include "metric/m4dMetric.h"
#include "metric/m4dMetricDatabase.h"


extern std::vector<Gvsm4dMetricDummy*>  gpMetric;
extern std::map<std::string,GvsTypeID>  gpTypeID;

/**
 * @brief gvsP_init_metric
 * @param sc
 * @param args
 * @return
 */
pointer gvsP_init_metric (scheme *sc, pointer args)
{
    m4d::MetricDatabase* database = new m4d::MetricDatabase();
#ifdef GVS_VERBOSE
    std::cerr << "\n..........gvsP_init_metric..........\n";
#endif
    if (args == sc->NIL) scheme_error("init-metric: no arguments");
    if (!is_pair(args)) scheme_error("init-metric: less arguments");


    std::string allowedNames[2] = {"type","id"};
    GvsParseAllowedNames allowedTypes[2] = {{gp_string_string,0},
                                            {gp_string_string,0}
                                           };

    GvsParseScheme* gvsParser = new GvsParseScheme(sc,allowedNames,allowedTypes,2);
    args = gvsParser->parse(args);

    // Read metric name
    std::string metricName;
    if (!gvsParser->getParameter("type",metricName)) {
        scheme_error("init-metric: type is missing!");
    }

    m4d::enum_metric i = database->getMetricNr(metricName);
    bool metricFound = (i!=m4d::enum_metric_unknown);
    if (!metricFound) {
        fprintf(stderr,"metric not available!\n");
        exit(0);
    }

    // Initialize metric
#ifdef GVS_VERBOSE
    printf("\n-->Initialize metric...\n");
#endif
    m4d::Metric* cnMetric;
    if (metricFound) {
        cnMetric = database->getMetric(i);
    }
    Gvsm4dMetricDummy* currMetric = new  Gvsm4dMetricDummy( cnMetric );

    // Determine number of metric parameters
    int numParam = currMetric->m4dMetric->getNumParams();

    std::string paramName;
    GvsDataType datType;
    GvsParseParamType paramType;
    int paramDim;

#ifdef GVS_VERBOSE
    printf("-->Read parameter %i:\n",numParam);
#endif

    bool valFound;
    double dvalue;
    std::string tvalue;

    std::vector<std::string> paramNames;

    currMetric->m4dMetric->getParamNames(paramNames);

    for (int i = 0; i < numParam; i++) {
        paramName = paramNames[i];  // currMetric->getParamName(i);
        datType   = gvsDT_DOUBLE;   // only double values are allowed

        gvsParser->transType(datType,paramType,paramDim);
        gvsParser->setAllowedName(paramName,paramType,paramDim);

        currMetric->AddParam(paramName,datType);

        valFound = false;
        if (datType == gvsDT_DOUBLE) {
            valFound  = gvsParser->getParameter((getLowCase(paramName)).c_str(),dvalue);
        } else if (datType == gvsDT_STRING) {
            valFound  = gvsParser->getParameter((getLowCase(paramName)).c_str(),tvalue);
        }

        if (valFound) {
            if (datType == gvsDT_DOUBLE) {
#ifdef GVS_VERBOSE
                printf("\t %12s = %f\n",paramName.c_str(),dvalue);
#endif
                currMetric->SetParam(paramName,dvalue);
            }
            else if (datType == gvsDT_STRING) {
#ifdef GVS_VERBOSE
                printf("\t %12s = %s Parameter als String nicht gueltig in m4d.\n",paramName.c_str(),tvalue.c_str());
#endif
            }
        }
#ifdef GVS_VERBOSE
        else
        {
            if (datType == gvsDT_DOUBLE)
            {
                currMetric->GetParam(paramName,dvalue);
                printf("\t %12s = %f  (Standardwert wird verwendet.) \n",paramName.c_str(),dvalue);
            }
            else if (datType == gvsDT_STRING)
            {
                //currMetric->getParam(paramName,tvalue);
                //printf("\t %12s = %s  (Standardwert wird verwendet.) \n",paramName.c_str(),tvalue.c_str());
            }
        }
#endif
    }
    gvsParser->testParamNames("init-metric");
    //currMetric->Print();
    gpMetric.push_back(currMetric);

    // Write an entry into object-map, where the metric can be found in th gpMetric vector.
    std::string idname = "unknown";
    if (!gvsParser->getParameter("id",idname)) {
        appendNum(idname,gpTypeID.size());
    }
    else if (gpTypeID.find(idname)!=gpTypeID.end()) {
        scheme_error("init-metric: ID already assigned!");
    }

    GvsTypeID tid = {gtMetric,static_cast<int>(gpMetric.size())-1,gpMetric[gpMetric.size()-1]};
    gpTypeID.insert(std::pair<std::string,GvsTypeID>(idname,tid));

    delete database;
    delete gvsParser;

    pointer R = ((sc->vptr->mk_symbol)(sc, "gtMetric"));
    return R;
}
