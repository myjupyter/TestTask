#include <iostream>
#include <string>

#include <speaker.hh>


static void speak(Speaker_ptr ref) {
    std::string buffer;
    if (CORBA::is_nil(ref)) {
        std::cerr << "Passed through the func nil ref" << std::endl;
        return;
    }
    while (std::getline(std::cin, buffer, '\n')) {
        CORBA::String_var src = buffer.c_str();
        ref->speak(src);
    }
}

static CORBA::Object_ptr getObjectReference(CORBA::ORB_ptr orb) {
    CosNaming::NamingContext_var rootContext;

    try {
        CORBA::Object_var obj;
        obj = orb->resolve_initial_references("NameService");

        rootContext = CosNaming::NamingContext::_narrow(obj);

        if (CORBA::is_nil(rootContext)) {
            std::cerr << "Failed to narrow the root naming context." << std::endl;
            return CORBA::Object::_nil();
        }
    } catch (CORBA::NO_RESOURCES&) {
        std::cerr << "Caught NO_RESOURCES exception. You must configure omniORB"
                  << "with the location of the naming service." << std::endl;
        return CORBA::Object::_nil();
    } catch (CORBA::ORB::InvalidName& ex) {
        std::cerr << "Service required is invalid [does not exist]." << std::endl;
        return CORBA::Object::_nil();
    }

    CosNaming::Name name;
    name.length(2);

    name[0].id = (const char*) "test";
    name[0].kind = (const char*) "my_context";
    name[1].id = (const char*) "Echo";
    name[1].kind = (const char*) "Object";

    try {
        return rootContext->resolve(name);
    } catch (CosNaming::NamingContext::NotFound& ex) {
        std::cerr << "Context not found." << std::endl;
    } catch (CORBA::TRANSIENT& ex) {
        std::cerr << "Caught system exception TRANSIENT -- unable to contact the "
                  << "naming service." << std::endl 
                  << "Make sure the naming server is running and that omniORB is"
                  << "configured correctly." << std::endl;
    } catch (CORBA::SystemException& ex) {
        std::cerr << "Caught a CORBA::" << ex._name() 
                  << " while using the naming service." << std::endl;
    }
    return CORBA::Object::_nil();
}

int main(int argc, char* argv[]) {
    try {
        CORBA::ORB_var orb = CORBA::ORB_init(argc, argv);
        CORBA::Object_var obj = getObjectReference(orb);

        Speaker_var ref = Speaker::_narrow(obj);

        ::speak(ref);

        orb->destroy();
    } catch (CORBA::TRANSIENT&) {
        std::cerr << "Caught system exception TRANSIENT -- unable to contact the server." 
                  << std::endl;
    } catch (CORBA::SystemException& ex) {
        std::cerr << "Caught a CORBA::" << ex._name() << std::endl;
    } catch (CORBA::Exception& ex) {
        std::cerr << "Caught CORBA::Exception: " << ex._name() << std::endl;
    }

    return 0;
}
