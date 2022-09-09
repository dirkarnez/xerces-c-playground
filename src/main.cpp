#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/OutOfMemoryException.hpp>
#include <xercesc/framework/StdOutFormatTarget.hpp>
#include <iostream>

using namespace XERCES_CPP_NAMESPACE;

// ---------------------------------------------------------------------------
//  This is a simple class that lets us do easy (though not terribly efficient)
//  trancoding of char* data to XMLCh data.
// ---------------------------------------------------------------------------
class XStr
{
public:
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    XStr(const char *const toTranscode)
    {
        // Call the private transcoding method
        fUnicodeForm = XMLString::transcode(toTranscode);
    }

    ~XStr()
    {
        XMLString::release(&fUnicodeForm);
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    const XMLCh *unicodeForm() const
    {
        return fUnicodeForm;
    }

private:
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  fUnicodeForm
    //      This is the Unicode XMLCh format of the string.
    // -----------------------------------------------------------------------
    XMLCh *fUnicodeForm;
};

#define X(str) XStr(str).unicodeForm()

// ---------------------------------------------------------------------------
//  main
// ---------------------------------------------------------------------------

int main(int argC, char *[])
{
    // Initialize the XML4C2 system.
    try
    {
        XMLPlatformUtils::Initialize();
    }

    catch (const XMLException &toCatch)
    {
        char *pMsg = XMLString::transcode(toCatch.getMessage());
        std::cerr << "Error during Xerces-c Initialization.\n"
                  << "  Exception message:"
                  << pMsg;
        XMLString::release(&pMsg);
        return 1;
    }

    // Watch for special case help request
    int errorCode = 0;
    if (argC > 1)
    {
        std::cout << "\nUsage:\n"
                     "    CreateDOMDocument\n\n"
                     "This program creates a new DOM document from scratch in memory.\n"
                     "It then prints the count of elements in the tree.\n"
                  << std::endl;
        errorCode = 1;
    }
    if (errorCode)
    {
        XMLPlatformUtils::Terminate();
        return errorCode;
    }

    {
        //  Nest entire test in an inner block.
        //  The tree we create below is the same that the XercesDOMParser would
        //  have created, except that no whitespace text nodes would be created.

        // <company>
        //     <product>Xerces-C</product>
        //     <category idea='great'>XML Parsing Tools</category>
        //     <developedBy>Apache Software Foundation</developedBy>
        // </company>

        DOMImplementation *impl = DOMImplementationRegistry::getDOMImplementation(X("Core"));

        if (impl != NULL)
        {
            try
            {
                DOMDocument *doc = impl->createDocument(
                    0,            // root element namespace URI.
                    X("company"), // root element name
                    0);           // document type object (DTD).

                DOMElement *rootElem = doc->getDocumentElement();

                DOMElement *prodElem = doc->createElement(X("product"));
                rootElem->appendChild(prodElem);

                DOMText *prodDataVal = doc->createTextNode(X("Xerces-C"));
                prodElem->appendChild(prodDataVal);

                DOMElement *catElem = doc->createElement(X("category"));
                rootElem->appendChild(catElem);

                catElem->setAttribute(X("idea"), X("great"));

                DOMText *catDataVal = doc->createTextNode(X("XML Parsing Tools"));
                catElem->appendChild(catDataVal);

                DOMElement *devByElem = doc->createElement(X("developedBy"));
                rootElem->appendChild(devByElem);

                DOMText *devByDataVal = doc->createTextNode(X("Apache Software Foundation"));
                devByElem->appendChild(devByDataVal);

                //
                // Now count the number of elements in the above DOM tree.
                //

                const XMLSize_t elementCount = doc->getElementsByTagName(X("*"))->getLength();
                std::cout << "The tree just created contains: " << elementCount << " elements." << std::endl;

                static const XMLCh gLS[] = {chLatin_L, chLatin_S, chNull};
                DOMImplementationLS *impl = (DOMImplementationLS *)DOMImplementationRegistry::getDOMImplementation(gLS);
                DOMLSSerializer *writer = DOMImplementation::getImplementation()->createLSSerializer();
                DOMLSOutput *theOutputDesc = ((DOMImplementationLS *)impl)->createLSOutput();
                XMLCh *gOutputEncoding = 0;
                theOutputDesc->setEncoding(gOutputEncoding);
                XMLFormatTarget *myFormTarget = new StdOutFormatTarget();

                theOutputDesc->setByteStream(myFormTarget);
                writer->write(doc, theOutputDesc);
                writer->release();
                doc->release();
            }
            catch (const OutOfMemoryException &)
            {
                std::cerr << "OutOfMemoryException" << std::endl;
                errorCode = 5;
            }
            catch (const DOMException &e)
            {
                std::cerr << "DOMException code is:  " << e.code << std::endl;
                errorCode = 2;
            }
            catch (...)
            {
                std::cerr << "An error occurred creating the document" << std::endl;
                errorCode = 3;
            }
        } // (inpl != NULL)
        else
        {
            std::cerr << "Requested implementation is not supported" << std::endl;
            errorCode = 4;
        }
    }

    XMLPlatformUtils::Terminate();
    return errorCode;
}
