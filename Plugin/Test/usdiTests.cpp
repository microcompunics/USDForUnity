#define testsImpl
#include "usdiTests.h"

void TestExport(const char *filename);
void TestExportHighMesh(const char *filename);
void TestExportReference(const char *filename, const char *flatten);
bool TestImport(const char *path);

extern "C" {

testsAPI void TestMain(int argc, char *argv[])
{
    TestExport("TestExport.usda");
    TestExport("TestExport.usdc");
    TestExportHighMesh("HighMesh.usda");
    TestExportHighMesh("HighMesh.usdc");
    TestExportReference("TestReference.usda", "Flatten.usda");

    TestImport("TestExport.usda");
    TestImport("TestReference.usda");
}

} // extern "C"
