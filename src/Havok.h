#pragma once

namespace RE
{
	class hkpConvexVerticesShape : public hkpConvexShape
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkpConvexVerticesShape;

		struct FourVectors
		{
			hkVector4 x;
			hkVector4 y;
			hkVector4 z;
		};

		struct BuildConfig
		{
			bool createConnectivity;
			bool shrinkByConvexRadius;
			bool useOptimizedShrinking;
			float convexRadius;
			int32_t maxVertices;
			float maxRelativeShrink;
			float maxShrinkingVerticesDisplacement;
			float maxCosAngleForBevelPlanes;
		};
		static_assert(sizeof(BuildConfig) == 0x18);

		hkVector4 aabbHalfExtents; // 30
		hkVector4 aabbCenter; // 40
		hkArray<FourVectors> rotatedVertices; // 50
		int32_t numVertices; // 60
		void* externalObject; // 68
		void* getFaceNormals; // 70
		hkArray<hkVector4> planeEquations; // 78
		void* connectivity; // 88
	};
	static_assert(sizeof(hkpConvexVerticesShape) == 0x90);

	struct hkStridedVertices
	{
	public:
		hkStridedVertices() :
			numVertices(0)
		{}

		hkStridedVertices(const hkArrayBase<hkVector4>& a_vertices) { set(a_vertices); }

		hkStridedVertices(const hkVector4* a_vertices, int a_numVertices) { set(a_vertices, a_numVertices); }

		const float* vertices;
		int numVertices;
		int striding;

		inline void set(const hkArrayBase<hkVector4>& a_vertices)
		{
			set(a_vertices.begin(), a_vertices.size());
		}

		template <typename T>
		inline void set(const T* a_vertices, int a_numVertices)
		{
			vertices = (const float*)a_vertices;
			numVertices = a_numVertices;
			striding = sizeof(T);
		}
	};

	class hkMemoryRouter
	{
	public:
		uint64_t unk00;             // 00
		uint64_t unk08;             // 08
		uint64_t unk10;             // 10
		uint64_t unk18;             // 18
		uint64_t unk20;             // 20
		uint64_t unk28;             // 28
		uint64_t unk30;             // 30
		uint64_t unk38;             // 38
		uint64_t unk40;             // 40
		uint64_t unk48;             // 48
		hkMemoryAllocator* temp;    // 50
		hkMemoryAllocator* heap;    // 58
		hkMemoryAllocator* debug;   // 60
		hkMemoryAllocator* solver;  // 68
		void* userData;             // 70
	};
	static_assert(offsetof(hkMemoryRouter, heap) == 0x58);

	class bhkCharacterControllerCinfo
	{
	public:
		inline static constexpr auto RTTI = RTTI_bhkCharacterControllerCinfo;

		// members
		uint64_t unk08;                                  // 08
		NiAVObject* object;                              // 10
		uint32_t collisionFilterInfo;                    // 18
		uint32_t unk1C;                                  // 1C
		uint64_t unk20;                                  // 20
		float acrobatics;                                // 28
		float unk2C;                                     // 2C
		hkpCharacterStateType characterStateType;        // 30
		uint64_t unk38;                                  // 38
		uint64_t unk40;                                  // 40
		hkpCharacterStateManager* characterStateManger;  // 48
		float unk50;                                     // 50
		float scale;                                     // 54
		uint32_t unk58;                                  // 58
		bool isNotPlayer;                                // 5C
		uint8_t unk5D;                                   // 5D
		uint16_t unk5E;                                  // 5E
		float center;                                    // 60
		uint32_t unk64;                                  // 64
		uint32_t unk68;                                  // 68
		uint32_t unk6C;                                  // 6C
		uint32_t unk70;                                  // 70
		uint32_t flags;                                  // 74
		uint64_t unk78;                                  // 78
		hkVector4 direction;                             // 80
		bhkShape* shape1;                                // 90
		bhkShape* shape2;                                // 98
		uint32_t unkA0;                                  // A0
		bool isChild;                                    // A4
		bool unkA5;                                      // A5
		uint16_t unkA6;                                  // A6
		uint64_t unkA8;                                  // A8
	};
	static_assert(sizeof(bhkCharacterControllerCinfo) == 0xB0);

	class hkpCharacterRigidBodyListener : public hkReferencedObject
	{
	};

	class hkpCharacterRigidBody :
		public hkReferencedObject,  // 00
		public hkpEntityListener,   // 10
		public hkpWorldPostSimulationListener  // 18
	{
	public:
		inline static constexpr auto RTTI = RTTI_hkpCharacterRigidBody;

		struct SupportInfo
		{
			hkContactPoint point;
			hkpRigidBody* rigidBody;
		};

		struct VertPointInfo
		{
			hkContactPoint vertPoint;
			hkpSimpleConstraintContactMgr* mgr;
		};

		~hkpCharacterRigidBody() override;  // 00

		// override (hkpEntityListener)
		void EntityAddedCallback(hkpEntity* a_entity) override;    // 01
		void EntityRemovedCallback(hkpEntity* a_entity) override;  // 02

		// override (hkpWorldPostSimulationListener)
		void PostSimulationCallback(hkpWorld* a_world) override;  // 01

		// add
		virtual void CheckSupport(const hkStepInfo& a_stepInfo, hkpSurfaceInfo& a_ground);                                                   // 03
		virtual hkpSurfaceInfo::SupportedState GetSupportInfo(const hkStepInfo& a_stepInfo, hkArray<SupportInfo>& a_supportInfo) const;      // 04
		virtual void GetGround(const hkArray<SupportInfo>& a_supportInfo, bool a_useDynamicBodyVelocities, hkpSurfaceInfo& a_ground) const;  // 05

		// members
		hkpRigidBody* character;                       // 20
		hkpCharacterRigidBodyListener* listener;       // 28
		hkVector4 up;                                  // 30
		float unweldingHeightOffsetFactor;             // 40
		float maxSlopeCosine;                          // 44
		float maxSpeedForSimplexSolver;                // 48
		float supportDistance;                         // 4C
		float hardSupportDistance;                     // 50
		hkVector4 acceleration;                        // 60
		float maxForce;                                // 70
		hkArray<VertPointInfo> verticalContactPoints;  // 78
	};
	static_assert(sizeof(hkpCharacterRigidBody) == 0x90);

	class bhkCharacterRigidBody : public bhkSerializable
	{
	public:
		inline static constexpr auto RTTI = RTTI_bhkCharacterRigidBody;
		inline static auto Ni_RTTI = NiRTTI_bhkCharacterRigidBody;

		~bhkCharacterRigidBody() override;  // 00

		// override (bhkSerializable)
		const NiRTTI* GetRTTI() const override;                       // 02
		NiObject* CreateClone(NiCloningProcess& a_cloning) override;  // 17
		void LoadBinary(NiStream& a_stream) override;                 // 18
		void LinkObject(NiStream& a_stream) override;                 // 19
		bool RegisterStreamables(NiStream& a_stream) override;        // 1A
		void SaveBinary(NiStream& a_stream) override;                 // 1B
		void AdjustRefCount(bool a_increment) override;               // 26
		hkpWorld* GetWorld1() override;                               // 27
		ahkpWorld* GetWorld2() override;                              // 28
		void Unk_2B(void) override;                                   // 2B
		void Unk_2C(void) override;                                   // 2C
		void Unk_2E(void) override;                                   // 2E
		void Unk_2F(void) override;                                   // 2F
	};

	class bhkCharRigidBodyController :
		public bhkCharacterController,        // 000
		public hkpCharacterRigidBodyListener  // 330
	{
	public:
		inline static constexpr auto RTTI = RTTI_bhkCharRigidBodyController;

		// members
		bhkCharacterRigidBody rigidBody;
	};
}

RE::hkMemoryRouter& hkGetMemoryRouter();
inline void* hkHeapAlloc(int numBytes) { return hkGetMemoryRouter().heap->BlockAlloc(numBytes); }
