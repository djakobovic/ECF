#include <ECF/ECF.h>
#include <vcclr.h>
#include <msclr\marshal_cppstd.h>
using namespace Tree::Primitives;
using namespace SpaceFlight::Simulation;
using namespace msclr::interop;
#define _AFXDLL

class SpaceFlightEvalOp : public EvaluateOp
{
public:
  void registerParameters(StateP state)
  {
    state->getRegistry()->registerEntry("testcases.dir", static_cast<voidP>(new std::string), ECF::STRING);
  }

  bool initialize(StateP state)
  {
    if (!state->getRegistry()->isModified("testcases.dir"))
    {
      return false;
    }
    try
    {
      std::string *dir = static_cast<std::string *>(state->getRegistry()->getEntry("testcases.dir").get());
      evaluator = gcnew Evaluator(marshal_as<System::String ^>(*dir));
      return true;
    }
    catch (System::Exception ^e)
    {
      printf("Error: %s\n", marshal_as<std::string>(e->Message).c_str());
      return false;
    }
  }

  FitnessP evaluate(IndividualP individual)
  {
    TreeP tree = boost::dynamic_pointer_cast<Tree::Tree>(individual->getGenotype());
    std::string nodes;
    nodes.reserve(1500);
    for (unsigned i = 0; i < tree->size(); ++i)
    {
      nodes += (*tree)[i]->primitive_->getName();
      nodes += ' ';
    }

    AiShipController ^controller = gcnew AiShipController(marshal_as<System::String ^>(nodes));
    FitnessP fitness = static_cast<FitnessP>(new FitnessMax);
    fitness->setValue(evaluator->Evaluate(controller) * 1000);
    return fitness;
  }

private:
  gcroot<Evaluator ^> evaluator;
};

class IfLessThen : public Primitive
{
public:
  IfLessThen()
  {
    nArguments_ = 4;
    name_ = "iflt";
  }

  void execute(void *result, Tree::Tree &tree)
  {
  }
};

int main(int argc, char **argv)
{
  TreeP tree = static_cast<TreeP>(new Tree::Tree);
  tree->addFunction(static_cast<PrimitiveP>(new IfLessThen));

  StateP state = static_cast<StateP>(new State);
  state->addGenotype(tree);
  state->setEvalOp(static_cast<EvaluateOpP>(new SpaceFlightEvalOp));
  state->initialize(argc, argv);
  state->run();
  return 0;
}