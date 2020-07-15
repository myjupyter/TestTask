#include <iostream>
#include <string>
#include <cstring>
#include <stdexcept>
#include <cstdio>
#include <cerrno>

#include <unistd.h>
#include <termios.h>

#include <speaker.hh>


int getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) {
        throw std::runtime_error(std::strerror(errno));
    }
    
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    
    if (tcsetattr(0, TCSANOW, &old) < 0) {
        throw std::runtime_error(std::strerror(errno));
    }
    if (::read(0, &buf, 1) < 0) {
        return -1;
    }
    std::cout << buf;
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) {
        throw std::runtime_error(std::strerror(errno));
    }
    return static_cast<int>(buf);
}

static void speak(Speaker_ptr ref) {
    if (CORBA::is_nil(ref)) {
        std::cerr << "Passed through the func nil ref" << std::endl;
        return;
    }
    int c = 0;
    while ((c = getch()) != -1) {
        std::string buffer(2, '\0');
        buffer[0] = static_cast<char>(c);
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
    setvbuf(stdin, (char*)NULL, _IONBF, 0);
    setvbuf(stdout, (char*)NULL, _IONBF, 0);
    
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
    } catch (std::runtime_error& ex) {
        std::cerr << ex.what() << std::endl;
    }

    return 0;
}
