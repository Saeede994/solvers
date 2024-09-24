    Info<< "Reading transportProperties\n" << endl;

    IOdictionary transportProperties
    (
        IOobject
        (
            "transportProperties",
            runTime.constant(),
            mesh,
            IOobject::MUST_READ_IF_MODIFIED,
            IOobject::NO_WRITE
        )
    );

    dimensionedScalar nu
    (
        transportProperties.lookup("nu")
    );

    Info<< "Writing cell volumes\n" << endl;
    volScalarField Vol
    (
        IOobject
        (
            "Vol",
            runTime.timeName(),
            mesh,
            IOobject::NO_READ,
            IOobject::AUTO_WRITE
        ),
	mesh,
	dimensionedScalar("zero",dimVolume,0.0)
    );
    Vol.ref() = mesh.V();
    Vol.write();

    Info<< "Writing continuityResidual\n" << endl;
    volScalarField continuityResidual
    (
        IOobject
        (
            "continuityResidual",  // Name of the output file
            runTime.timeName(),     // Directory to save the field in
            mesh,                   // The mesh for the field
            IOobject::NO_READ,      // Do not read from file
            IOobject::AUTO_WRITE    // Automatically write the field to the time directories
        ),
        mesh,  // The mesh to initialize the field
        dimensionedScalar("zero", dimless, 0.0)  // Initial value of 0
    );
    continuityResidual.write();



    Info<< "Reading field p\n" << endl;
    volScalarField p
    (
        IOobject
        (
            "p",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh
    );

    Info<< "Reading field U\n" << endl;
    volVectorField U
    (
        IOobject
        (
            "U",
            runTime.timeName(),
            mesh,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        mesh
    );

    Info<< "Reading/calculating face flux field phi\n" << endl;
    surfaceScalarField phi
    (
        IOobject
        (
            "phi",
            runTime.timeName(),
            mesh,
            IOobject::READ_IF_PRESENT,
            IOobject::AUTO_WRITE
        ),
        linearInterpolate(U) & mesh.Sf()
    );

    label pRefCell = 0;
    scalar pRefValue = 0.0;
    setRefCell(p, mesh.solutionDict().subDict("SIMPLE"), pRefCell, pRefValue);

    singlePhaseTransportModel laminarTransport(U, phi);

    autoPtr<incompressible::turbulenceModel> turbulence
    (
        incompressible::turbulenceModel::New(U, phi, laminarTransport)
    );
