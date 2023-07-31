/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2017 OpenFOAM Foundation
    Copyright (C) 2019 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Application
    laplacianFoam

Group
    grpBasicSolvers

Description
    Laplace equation solver for a scalar quantity.

    \heading Solver details
    The solver is applicable to, e.g. for thermal diffusion in a solid.  The
    equation is given by:

    \f[
        \ddt{T}  = \div \left( D_T \grad T \right)
    \f]

    Where:
    \vartable
        T     | Scalar field which is solved for, e.g. temperature
        D_T   | Diffusion coefficient
    \endvartable

    \heading Required fields
    \plaintable
        T     | Scalar field which is solved for, e.g. temperature
    \endplaintable

\*---------------------------------------------------------------------------*/

#include "fvCFD.H"
#include "fvOptions.H"
#include "simpleControl.H"
#include "func1.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

int main(int argc, char *argv[])
{

    label finish = 1;
    
    while (finish > 0)
    {
        function1(); 
    }

    argList::addNote
    (
        "Laplace equation solver for a scalar quantity."
    );

    #include "postProcess.H"

    #include "addCheckCaseOptions.H"
    #include "setRootCaseLists.H"
    #include "createTime.H"
    #include "createMesh.H"

    simpleControl simple(mesh);

    #include "createFields.H"

    // * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

    Info<< "\nCalculating temperature distribution\n" << endl;

    while (simple.loop())
    {
        Info<< "Time = " << runTime.timeName() << nl << endl;

        scalarField T_test (T.internalField().size(), 0);

        vectorField v_test (T.internalField().size(), vector(0, 0, 0));

        forAll(T, cellI)
        {
            T_test[cellI] = T[cellI]*T[cellI];

            v_test[cellI] =  vector(cellI, cellI, cellI) ;

            if (cellI == 30)
            {
                T[cellI] = 273;
                Info <<"I have changed the value of cell 30" << endl;
            }
        }

        while (simple.correctNonOrthogonal())
        {
            fvScalarMatrix TEqn
            (
                fvm::ddt(T) - fvm::laplacian(DT, T)
             ==
                fvOptions(T)
            );

            fvOptions.constrain(TEqn);
            TEqn.solve();
            fvOptions.correct(T);
        }

        #include "write.H"

        runTime.printExecutionTime(Info);

        // Memory debugging 
            // Allocated memory does not get freed
            // scalarField* T_test2Ptr = new scalarField(T_test.size(), 0);
            // Info << *T_test2Ptr << endl;
            // // delete T_test2Ptr; // solution

            // Dynamic allocated array
            // scalar* p = new scalar[5]{5,4,3,2,1};
            // // delete[] p; // solution for dynamically allocated array 

        // Memory should be deallocated in a way compatible with how it was allocated. 
        // If new[] was used, deallocate with delete[].
        // If new was used, deallocate with delete. 

        // Uninitialized variables
            // scalar volume;
            // // scalar volume{0}; // solution
            // Info  << volume << endl;
            // if (volume > 0)
            //     Info << "I am higher than 1" << endl;
            // else if (volume == 0)
            //     Info << "I am lower than 1" << endl;

        // Segmentation fault
        // Segmentation faults in C++ occurs when a program attempts to access a part of memory that it is not allowed to access. 

        // Out of bounds. 
        // label* labelPtr = new label[4]{4,3,2,1};
        // std::cout << labelPtr[0] << std::endl;
        // labelPtr[5] = 12; // out of bounds
        // delete[] labelPtr;

        // Dereference of a null pointer
        // label* labelPtr = NULL; 
        // *labelPtr = 2;

        // // Using after freeing the memory
        // label* labelPtr = new label[4]{4,3,2,1};
        // delete[] labelPtr;
        // labelPtr[0] = 12;
    }

    Info<< "End\n" << endl;

    return 0;
}

// ************************************************************************* //

