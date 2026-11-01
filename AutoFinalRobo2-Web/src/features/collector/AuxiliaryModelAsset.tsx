import { useMemo } from 'react';
import { useLoader } from '@react-three/fiber';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import type { AuxiliaryModelDefinition } from './auxiliaryModels';

/** One cached geometry/material set, a distinct transform tree per instance. */
export function AuxiliaryModelAsset({ model }: { model: AuxiliaryModelDefinition }) {
  const asset = useLoader(GLTFLoader, model.url);
  const scene = useMemo(() => asset.scene.clone(true), [asset.scene]);
  return <primitive object={scene} dispose={null} />;
}
