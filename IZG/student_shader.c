/*!
 * @file 
 * @brief This file contains implemenation of phong vertex and fragment shader.
 *
 * @author Tomáš Milet, imilet@fit.vutbr.cz
 */

#include<math.h>
#include<assert.h>

#include"student/student_shader.h"
#include"student/gpu.h"
#include"student/uniforms.h"

/// \addtogroup shader_side Úkoly v shaderech
/// @{

void phong_vertexShader(
    GPUVertexShaderOutput     *const output,
    GPUVertexShaderInput const*const input ,
    GPU                        const gpu   ){

	Uniforms const unifHandle = gpu_getUniformsHandle(gpu);
	UniformLocation const viewMatrixLocation = getUniformLocation(gpu, "viewMatrix");
	UniformLocation const projectionMatrixLocation = getUniformLocation(gpu, "projectionMatrix");

	Mat4 const * viewMatrix = shader_interpretUniformAsMat4(unifHandle, viewMatrixLocation);
	Mat4 const * projectionMatrix = shader_interpretUniformAsMat4(unifHandle, projectionMatrixLocation);

	Vec3 const * position = vs_interpretInputVertexAttributeAsVec3(gpu, input, 0);
	Vec3 const * normal = vs_interpretInputVertexAttributeAsVec3(gpu, input, 1);

	Mat4 mat;
	multiply_Mat4_Mat4(&mat, projectionMatrix, viewMatrix);
	Vec4 pos;
	copy_Vec3Float_To_Vec4(&pos, position, 1.f);
	multiply_Mat4_Vec4(&output->gl_Position, &mat, &pos);
	Vec3 * position1 = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 0);
	Vec3 * normal1 = vs_interpretOutputVertexAttributeAsVec3(gpu, output, 1);

	position1->data[0] = position->data[0];
	position1->data[1] = position->data[1];
	position1->data[2] = position->data[2];
	normal1->data[0] = normal->data[0];
	normal1->data[1] = normal->data[1];
	normal1->data[2] = normal->data[2];


}

void phong_fragmentShader(
    GPUFragmentShaderOutput     *const output,
    GPUFragmentShaderInput const*const input ,
    GPU                          const gpu   ){
  /// \todo Naimplementujte fragment shader, který počítá phongův osvětlovací model s phongovým stínováním.<br>
  /// <b>Vstup:</b><br>
  /// Vstupní fragment by měl v nultém fragment atributu obsahovat interpolovanou pozici ve world-space a v prvním
  /// fragment atributu obsahovat interpolovanou normálu ve world-space.<br>
  /// <b>Výstup:</b><br> 
  /// Barvu zapište do proměnné color ve výstupní struktuře.<br>
  /// <b>Uniformy:</b><br>
  /// Pozici kamery přečtěte z uniformní proměnné "cameraPosition" a pozici světla přečtěte z uniformní proměnné "lightPosition".
  /// Zachovejte jména uniformních proměnný.
  /// Pokud tak neučiníte, akceptační testy selžou.<br>
  /// <br>
  /// Dejte si pozor na velikost normálového vektoru, při lineární interpolaci v rasterizaci může dojít ke zkrácení.
  /// Zapište barvu do proměnné color ve výstupní struktuře.
  /// Shininess faktor nastavte na 40.f
  /// Difuzní barvu materiálu nastavte na čistou zelenou.
  /// Spekulární barvu materiálu nastavte na čistou bílou.
  /// Barvu světla nastavte na bílou.
  /// Nepoužívejte ambientní světlo.<br>
  /// <b>Seznam funkcí, které jistě využijete</b>:
  ///  - shader_interpretUniformAsVec3()
  ///  - fs_interpretInputAttributeAsVec3()
  (void)output;
  (void)input;
  Uniforms const unifHandle = gpu_getUniformsHandle(gpu);
  UniformLocation const cameraPositionLocation = getUniformLocation(gpu, "cameraPosition");
  UniformLocation const lightPositionLocation = getUniformLocation(gpu, "lightPosition");
  Vec3 const * cameraPosition = shader_interpretUniformAsVec3(unifHandle, cameraPositionLocation);
  Vec3 const * lightPosition = shader_interpretUniformAsVec3(unifHandle, lightPositionLocation);
  Vec3 const * position = fs_interpretInputAttributeAsVec3(gpu, input, 0);
  Vec3 const * normal = fs_interpretInputAttributeAsVec3(gpu, input, 1);
}

/// @}
