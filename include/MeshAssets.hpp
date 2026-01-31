#pragma once

#include "Mesh.hpp"

namespace RS
{
	RS::Mesh GetRectMesh()
	{
		auto rectMesh = RS::Mesh();
		rectMesh.vertices.reserve(4);
		rectMesh.vertices.emplace_back(Gadget::Vector4(-0.5, -0.5, 0.0, 1.0));
		rectMesh.vertices.emplace_back(Gadget::Vector4(-0.5, 0.5, 0.0, 1.0));
		rectMesh.vertices.emplace_back(Gadget::Vector4(0.5, -0.5, 0.0, 1.0));
		rectMesh.vertices.emplace_back(Gadget::Vector4(0.5, 0.5, 0.0, 1.0));

		rectMesh.indices.push_back(0);
		rectMesh.indices.push_back(1);
		rectMesh.indices.push_back(2);

		rectMesh.indices.push_back(2);
		rectMesh.indices.push_back(1);
		rectMesh.indices.push_back(3);

		rectMesh.vertices[0].color = Gadget::Color(1.0, 0.0, 0.0, 1.0);
		rectMesh.vertices[1].color = Gadget::Color(0.0, 1.0, 0.0, 1.0);
		rectMesh.vertices[2].color = Gadget::Color(0.0, 0.0, 1.0, 1.0);
		rectMesh.vertices[3].color = Gadget::Color(0.0, 0.0, 0.0, 1.0);

		return rectMesh;
	}

	RS::Mesh GetCubeMesh()
	{
		auto cubeMesh = RS::Mesh();
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.0, -1.0, -1.0, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, 1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, -1.f, 1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, 1.f, 1.f, 1.0));

		// +X face
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, -1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, 1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, -1.f, 1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, 1.f, 1.f, 1.0));

		// -Y face
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, -1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, -1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, -1.f, 1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, -1.f, 1.f, 1.0));

		// +Y face
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, 1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, 1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, 1.f, 1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, 1.f, 1.f, 1.0));

		// -Z face
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, -1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, -1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, 1.f, -1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, 1.f, -1.f, 1.0));

		// +Z face
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, -1.f, 1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, -1.f, 1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.f, 1.f, 1.f, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.f, 1.f, 1.f, 1.0));

		cubeMesh.vertices.push_back(Gadget::Vector4(-1.0, -1.0, -1.0, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.0, -1.0, 1.0, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.0, 1.0, -1.0, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(-1.0, 1.0, 1.0, 1.0));

		cubeMesh.vertices.push_back(Gadget::Vector4(1.0, -1.0, -1.0, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.0, -1.0, 1.0, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.0, 1.0, -1.0, 1.0));
		cubeMesh.vertices.push_back(Gadget::Vector4(1.0, 1.0, 1.0, 1.0));

		cubeMesh.indices.push_back(0); cubeMesh.indices.push_back(2); cubeMesh.indices.push_back(1);
		cubeMesh.indices.push_back(1); cubeMesh.indices.push_back(2); cubeMesh.indices.push_back(3);

		cubeMesh.indices.push_back(4); cubeMesh.indices.push_back(5); cubeMesh.indices.push_back(6);
		cubeMesh.indices.push_back(6); cubeMesh.indices.push_back(5); cubeMesh.indices.push_back(7);

		cubeMesh.indices.push_back(8); cubeMesh.indices.push_back(9); cubeMesh.indices.push_back(10);
		cubeMesh.indices.push_back(10); cubeMesh.indices.push_back(9); cubeMesh.indices.push_back(11);

		cubeMesh.indices.push_back(12); cubeMesh.indices.push_back(14); cubeMesh.indices.push_back(13);
		cubeMesh.indices.push_back(14); cubeMesh.indices.push_back(15); cubeMesh.indices.push_back(13);

		cubeMesh.indices.push_back(16); cubeMesh.indices.push_back(18); cubeMesh.indices.push_back(17);
		cubeMesh.indices.push_back(17); cubeMesh.indices.push_back(18); cubeMesh.indices.push_back(19);

		cubeMesh.indices.push_back(20); cubeMesh.indices.push_back(21); cubeMesh.indices.push_back(22);
		cubeMesh.indices.push_back(21); cubeMesh.indices.push_back(23); cubeMesh.indices.push_back(22);


		cubeMesh.vertices[0].color = Gadget::Color(0.f, 1.f, 1.f, 1.f);
		cubeMesh.vertices[1].color = Gadget::Color(0.f, 1.f, 1.f, 1.f);
		cubeMesh.vertices[2].color = Gadget::Color(0.f, 1.f, 1.f, 1.f);
		cubeMesh.vertices[3].color = Gadget::Color(0.f, 1.f, 1.f, 1.f);

		// +X face
		cubeMesh.vertices[4].color = Gadget::Color(1.f, 0.f, 0.f, 1.f);
		cubeMesh.vertices[5].color = Gadget::Color(1.f, 0.f, 0.f, 1.f);
		cubeMesh.vertices[6].color = Gadget::Color(1.f, 0.f, 0.f, 1.f);
		cubeMesh.vertices[7].color = Gadget::Color(1.f, 0.f, 0.f, 1.f);

		// -Y face
		cubeMesh.vertices[8].color = Gadget::Color(1.f, 0.f, 1.f, 1.f);
		cubeMesh.vertices[9].color = Gadget::Color(1.f, 0.f, 1.f, 1.f);
		cubeMesh.vertices[10].color = Gadget::Color(1.f, 0.f, 1.f, 1.f);
		cubeMesh.vertices[11].color = Gadget::Color(1.f, 0.f, 1.f, 1.f);

		// +Y face
		cubeMesh.vertices[12].color = Gadget::Color(0.f, 1.f, 0.f, 1.f);
		cubeMesh.vertices[13].color = Gadget::Color(0.f, 1.f, 0.f, 1.f);
		cubeMesh.vertices[14].color = Gadget::Color(0.f, 1.f, 0.f, 1.f);
		cubeMesh.vertices[15].color = Gadget::Color(0.f, 1.f, 0.f, 1.f);

		// -Z face
		cubeMesh.vertices[16].color = Gadget::Color(1.f, 1.f, 0.f, 1.f);
		cubeMesh.vertices[17].color = Gadget::Color(1.f, 1.f, 0.f, 1.f);
		cubeMesh.vertices[18].color = Gadget::Color(1.f, 1.f, 0.f, 1.f);
		cubeMesh.vertices[19].color = Gadget::Color(1.f, 1.f, 0.f, 1.f);

		// +Z face
		cubeMesh.vertices[20].color = Gadget::Color(0.f, 0.f, 1.f, 1.f);
		cubeMesh.vertices[21].color = Gadget::Color(0.f, 0.f, 1.f, 1.f);
		cubeMesh.vertices[22].color = Gadget::Color(0.f, 0.f, 1.f, 1.f);
		cubeMesh.vertices[23].color = Gadget::Color(0.f, 0.f, 1.f, 1.f);

		return cubeMesh;
	}
}
