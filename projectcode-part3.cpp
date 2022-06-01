radiologyDepartment* secretaryCommand::radiology=NULL;
vector<labDepartment*>* secretaryCommand::labDepartments=NULL;
class askForClinics:public secretaryCommand {
public:
    askForClinics(baseClinic *pClinic, patient *pPatient) : secretaryCommand(pClinic, pPatient) { }
    void execute() {
        cout<<requestingPatient->getName()<<" asked for clinic locations:"<<endl;
        if(clinic->getClinicName() != "radiology")
            cout<<"Radiology Clinic is located in:"<<endl;
        if(clinic->getClinicName() != "lab")
            cout<<"Lab Clinic is located in:"<<endl;
        if(clinic->getClinicName() != "cardiology")
            cout<<"Cardiology Clinic is located in:"<<endl;
        if(clinic->getClinicName() != "orthopedics")
            cout<<"Orthopedics Clinic is located in:"<<endl;
        if(clinic->getClinicName() != "endocrinology")
            cout<<"Endocrinology Clinic is located in:"<<endl;
    }
    string getCommandName(){return "askForClinics";}
};
class askForAnAppointment:public secretaryCommand
{
public:
    askForAnAppointment(baseClinic *pClinic, patient *pPatient) : secretaryCommand(pClinic, pPatient) {}
    void execute(){
        cout<<"An appointment is made in "<<clinic->getClinicName()<<" clinic for "<<requestingPatient->getName()<<endl;
    }
    string getCommandName(){return "askForAppointment";}
};

class seeDoctor:public secretaryCommand
{
public:
    seeDoctor(baseClinic *pClinic, patient *pPatient) : secretaryCommand(pClinic, pPatient) {}
    void execute(){
        cout<<"Doctor in "<<clinic->getClinicName()<<" clinic is ready to see "<<requestingPatient->getName()<<endl;
        vector<baseTest*>* testsOfPatient=requestingPatient->getTestsHaveDone();
        for(int i=0;i<testsOfPatient->size();i++)
        {
            clinic->getAssignedDoctor()->prescribe(testsOfPatient->at(i));
            drugInfo* drugInfoToAdd=clinic->getAssignedDoctor()->getPrescribedDrug();
            requestingPatient->addDrugInfo(drugInfoToAdd);
            cout<<"Patient received the drug."<<endl;
        }
    }
    string getCommandName(){return "seeDoctor";}
};

class checkTests:public secretaryCommand
{
public:
    checkTests(baseClinic *pClinic, patient *pPatient) : secretaryCommand(pClinic, pPatient) {}
    void execute(){
        vector<baseTest*>* currentTests=requestingPatient->getTestsHaveDone();
        vector<baseTestRequest*>* requiredTestsOfClinic=clinic->getRequiredTests();
        cout<<"Secretary in "<<clinic->getClinicName()<<" clinic started to check tests for "<<requestingPatient->getName()<<endl;
        for(int i=0;i<requiredTestsOfClinic->size();i++)
        {
            cout<<clinic->getClinicName()<<" clinic require "<<requiredTestsOfClinic->at(i)->getTestName()<<" test."<<endl;
            bool testFound=false;
            for(int j=0;j<currentTests->size();j++)
            {
                if(requiredTestsOfClinic->at(i)->getTestName()==currentTests->at(j)->getTestName())
                {
                    testFound=true;
                    cout<<requestingPatient->getName()<<" has done "<<requiredTestsOfClinic->at(i)->getTestName()<<" test before."<<endl;
                    break;
                }
            }
            if(!testFound)
            {
                cout<<requestingPatient->getName()<<" haven't done "<<requiredTestsOfClinic->at(i)->getTestName()<<" test before."<<endl;
                baseTestDepartment* departmentToTestWith;
                if(requiredTestsOfClinic->at(i)->getType()=="radiological")
                {
                    departmentToTestWith=radiology;
                }
                else if(requiredTestsOfClinic->at(i)->getType()=="blood")
                {
                    departmentToTestWith=labDepartments->at(0);
                }
                cout<<departmentToTestWith->getExpectedTestType()<<" test is about to be done"<<endl;
                departmentToTestWith->createTest(requiredTestsOfClinic->at(i));
                currentTests->push_back(departmentToTestWith->getTestResult());
                cout<<"Patient successfully received the test result."<<endl;
            }
        }
    }
    string getCommandName(){return "checkTests";}
};
class secretary {
private:
    vector<secretaryCommand*> previousCommandsByCurrentPatient;
    patient* currentPatient;
    baseClinic* assignedClinic;
public:
    secretary(baseClinic* Clinic) {
        assignedClinic=Clinic;
    }
    ~secretary() {
        previousCommandsByCurrentPatient.erase(previousCommandsByCurrentPatient.begin(),previousCommandsByCurrentPatient.end());
    }
    void acceptNewPatient(patient* newPatient) {
        currentPatient=newPatient;
        cout<<"Secretary from "<<assignedClinic->getClinicName()<<" clinic accepted "<<currentPatient->getName()<<" to hear his requests"<<endl;
        previousCommandsByCurrentPatient.erase(previousCommandsByCurrentPatient.begin(),previousCommandsByCurrentPatient.end());
    }
    void acceptRequest(secretaryCommand* newCommand) {
        if(newCommand->getCommandName()=="seeDoctor") {
            bool testsHaveBeenChecked=false;
            for(int i=0;i<previousCommandsByCurrentPatient.size();i++) {
                if(previousCommandsByCurrentPatient[i]->getCommandName()=="checkTests") {
                    testsHaveBeenChecked=true;
                    break;
                }
            }
            if(!testsHaveBeenChecked) {
                cout<<currentPatient->getName()<<" requested to see the doctor from "<<assignedClinic->getClinicName()<<". However patient's tests haven't been checked before. "<<endl;
                secretaryCommand* checkFromSecretary=new checkTests(assignedClinic,currentPatient);
                checkFromSecretary->execute();
                previousCommandsByCurrentPatient.push_back(checkFromSecretary);
            }
        }
        newCommand->execute();
        previousCommandsByCurrentPatient.push_back(newCommand);
    }
    baseClinic *getAssignedClinic() const {
        return assignedClinic;
    }
};
class drugRecord { 
private:
    vector<patient*> drugOwners[NUMBER_OF_DRUG_TYPES];
    map<int, string> nameForIndex;
public:
    ~drugRecord(){};
    drugRecord(){
        nameForIndex.insert(make_pair(0,"DrugA"));
        nameForIndex.insert(make_pair(1,"DrugB"));
        nameForIndex.insert(make_pair(2,"DrugC"));
        nameForIndex.insert(make_pair(3,"DrugD"));
    };
    void addPatientToRecord(patient* patientToAdd) { 
        vector<drugInfo*>* drugsOfPatient = patientToAdd->getDrugsPatientHolds();
        for(int i=0;i<drugsOfPatient->size();i++) {
            drugOwners[drugsOfPatient->at(i)->getDrugIndex()].push_back(patientToAdd);
        }
    }
    void releasePatientFromRecord(patient* patientToLeave){ 
        vector<drugInfo*>* drugsOfPatient=patientToLeave->getDrugsPatientHolds();
        for(int i=0;i<drugsOfPatient->size();i++) {
            for (unsigned int i = 0;i  < drugOwners[drugsOfPatient->at(i)->getDrugIndex()].size() ; i++) {
                if(drugOwners[drugsOfPatient->at(i)->getDrugIndex()][i]->getEmail() == patientToLeave->getEmail()){
                    drugOwners[drugsOfPatient->at(i)->getDrugIndex()].erase(drugOwners[drugsOfPatient->at(i)->getDrugIndex()].begin()+i);
                    return;
                }
            }
        }
    }
    void informAllPatients(int whichDrug) { 
        for(int i=0;i<drugOwners[whichDrug].size();i++){
            drugOwners->at(i)->Update(whichDrug);
        }
    }
};
