#include <iostream>

#include <speaker.hh>

static CORBA::Boolean bindObjectToName(CORBA::ORB_ptr orb, CORBA::Object_ptr objref) {
    CosNaming::NamingContext_var rootContext;
    
    try {
        CORBA::Object_var obj = orb->resolve_initial_references("NameService");

        rootContext = CosNaming::NamingContext::_narrow(obj);
        if (CORBA::is_nil(rootContext)) {
            std::cerr << "Failed to narrow the root naming context." << std::endl; 
            return 0;
        }
    } catch (CORBA::ORB::InvalidName&) {
        std::cerr << "Service required is invalid [does not exist]." << std::endl;
        return 0;
    }

    try {
        CosNaming::Name contextName;
        contextName.length(1);
        contextName[0].id = (const char*) "test";
        contextName[0].kind = (const char*) "my_context";

        CosNaming::NamingContext_var testContext;
        try {
            testContext = rootContext->bind_new_context(contextName);
        } catch(CosNaming::NamingContext::AlreadyBound& ex) {
            CORBA::Object_var obj = rootContext->resolve(contextName);
            testContext = CosNaming::NamingContext::_narrow(obj);
            if (CORBA::is_nil(testContext)) {
                std::cerr << "Failed to narrow naming context." << std::endl;
                return 0;
            }
        }

        CosNaming::Name objectName;
        objectName.length(1);
        objectName[0].id = (const char*) "Echo";
        objectName[0].kind = (const char*) "Object";

        try {
            testContext->bind(objectName, objref);
        } catch (CosNaming::NamingContext::AlreadyBound& ex) {
            testContext->rebind(objectName, objref);
        }
    } catch (CORBA::TRANSIENT& ex) {
        std::cerr << "Caught system exception TRANSIENT -- unable to contact the "
                  << "naming service." << std::endl
                  << "Make sure the naming server is running and that omniORB is"
                  << "configured correctly." << std::endl;
        return 0;
    } catch (CORBA::SystemException& ex) {
        std::cerr << "Caught a CORBA::" << ex._name()
                  << " while using the naming service." << std::endl;
        return 0;
    }
    return 1;
}

class Speaker_i : public POA_Speaker {
    public:
        inline Speaker_i() = default;
        virtual ~Speaker_i() = default; 
        virtual void speak(const char* mesg) {
            std::cout << mesg << std::endl;
        }
};

int main(int argc, char* argv[]) { 
    try {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
        CORBA::Object_var obj = orb->resolve_initial_references("RootPOA");
        PortableServer::POA_var poa = PortableServer::POA::_narrow(obj);
        
        PortableServer::Servant_var<Speaker_i> speaker = new Speaker_i();
        PortableServer::ObjectId_var speaker_id = poa->activate_object(speaker);

        obj = speaker->_this();

        CORBA::String_var sior(orb->object_to_string(obj));
        std::cout << sior << std::endl;
        
        if (!bindObjectToName(orb, obj)) {
            return 1;
        }

        PortableServer::POAManager_var pman = poa->the_POAManager();
        pman->activate();

        orb->run();
    } catch (CORBA::SystemException& ex) {
        std::cerr << "Caught CORBA::" << ex._name() << std::endl;
    } catch (CORBA::Exception& ex) {
        std::cerr << "Caught CORBA::Exception: " << ex._name() << std::endl;
    } catch (...) {
        std::cerr << "Another exceptions has occured!" << std::endl;
    }
    return 0;
}
