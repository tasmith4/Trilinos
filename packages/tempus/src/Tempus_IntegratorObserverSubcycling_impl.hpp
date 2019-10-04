// @HEADER
// ****************************************************************************
//                Tempus: Copyright (2017) Sandia Corporation
//
// Distributed under BSD 3-clause license (See accompanying file Copyright.txt)
// ****************************************************************************
// @HEADER

#ifndef Tempus_IntegratorObserverSubcycling_impl_hpp
#define Tempus_IntegratorObserverSubcycling_impl_hpp

#include "Tempus_Stepper.hpp"

namespace Tempus {

template<class Scalar>
IntegratorObserverSubcycling<Scalar>::IntegratorObserverSubcycling(){}

template<class Scalar>
IntegratorObserverSubcycling<Scalar>::~IntegratorObserverSubcycling(){}

template<class Scalar>
void IntegratorObserverSubcycling<Scalar>::
observeStartIntegrator(const Integrator<Scalar>& integrator){

  const Teuchos::RCP<Teuchos::FancyOStream> out = integrator.getOStream();
  Teuchos::OSTab ostab(out,0,"ScreenOutput");
  *out << "\n    Begin Subcycling -------------------------------------------------------\n";
    // << "  Step       Time         dt  Abs Error  Rel Error  Order  nFail  dCompTime"
    // << std::endl;
}

template<class Scalar>
void IntegratorObserverSubcycling<Scalar>::
observeStartTimeStep(const Integrator<Scalar>& /* integrator */){}

template<class Scalar>
void IntegratorObserverSubcycling<Scalar>::
observeNextTimeStep(const Integrator<Scalar>& /* integrator */){}

template<class Scalar>
void IntegratorObserverSubcycling<Scalar>::
observeBeforeTakeStep(const Integrator<Scalar>& /* integrator */){}

template<class Scalar>
void IntegratorObserverSubcycling<Scalar>::
observeAfterTakeStep(const Integrator<Scalar>& /* integrator */){}

template<class Scalar>
void IntegratorObserverSubcycling<Scalar>::
observeAfterCheckTimeStep(const Integrator<Scalar>& /* integrator */){}

template<class Scalar>
void IntegratorObserverSubcycling<Scalar>::
observeEndTimeStep(const Integrator<Scalar>& integrator){

  using Teuchos::RCP;
  RCP<SolutionStateMetaData<Scalar> > csmd =
    integrator.getSolutionHistory()->getCurrentState()->getMetaData();

  if ((csmd->getOutputScreen() == true) or
      (csmd->getOutput() == true) or
      (csmd->getTime() == integrator.getTimeStepControl()->getFinalTime())) {

     const Scalar steppertime = integrator.getStepperTimer()->totalElapsedTime();
     // reset the stepper timer
     integrator.getStepperTimer()->reset();

     const Teuchos::RCP<Teuchos::FancyOStream> out = integrator.getOStream();
     Teuchos::OSTab ostab(out,0,"ScreenOutput");
     *out<<std::scientific
        <<std::setw( 6)<<std::setprecision(3)<<csmd->getIStep()
        <<std::setw(11)<<std::setprecision(3)<<csmd->getTime()
        <<std::setw(11)<<std::setprecision(3)<<csmd->getDt()
        <<std::setw(11)<<std::setprecision(3)<<csmd->getErrorAbs()
        <<std::setw(11)<<std::setprecision(3)<<csmd->getErrorRel()
        <<std::fixed     <<std::setw( 7)<<std::setprecision(1)<<csmd->getOrder()
        <<std::scientific<<std::setw( 7)<<std::setprecision(3)<<csmd->getNFailures()
        <<std::setw(11)<<std::setprecision(3)<<steppertime
        <<std::endl;
  }

}

template<class Scalar>
void IntegratorObserverSubcycling<Scalar>::
observeEndIntegrator(const Integrator<Scalar>& integrator){

  const Teuchos::RCP<Teuchos::FancyOStream> out = integrator.getOStream();
  Teuchos::OSTab ostab(out,0,"ScreenOutput");
  *out << "    End Subcycling ---------------------------------------------------------\n\n";
}

} // namespace Tempus
#endif // Tempus_IntegratorObserverSubcycling_impl_hpp
