import { useCallback, useEffect, useState, type CSSProperties } from 'react';
import { Link } from 'react-router-dom';
import { useLoader } from '@react-three/fiber';
import { GLTFLoader } from 'three/addons/loaders/GLTFLoader.js';
import { Button } from '../../components/ui/Button';
import { UiIcon } from '../../components/ui/UiIcon';
import { RenderBoundary } from '../../components/ui/RenderBoundary';
import { ModelAssetProgress } from '../../components/three/ModelAssetProgress';
import { clearRobotModelCache } from '../../components/three/RobotModel';
import { ProgressGLTFLoader, resetModelAsset } from '../../components/three/modelAssetLoading';
import { AssemblyScene, ASSEMBLY_ARM_URL } from './AssemblyScene';
import { ASSEMBLY_CHAPTERS, chapterIndex, clamp01, ease, segment } from './assemblyTimeline';
import { useAssemblyScroll } from './useAssemblyScroll';
import { useAssemblyPlayback } from './useAssemblyPlayback';
import { ASSEMBLY_DURATION } from './assemblyPlaybackTimeline';
import { AssemblyInterfaceShowcase } from './AssemblyInterfaceShowcase';
import { CUBE_MODEL } from '../collector/auxiliaryModels';
import { ADDITIONS_ASSET_URL } from './assemblyAdditions';
import '../../styles/assembly.css';

export function AssemblyPage() {
  const { track, stage, progress, seek: scrollSeek, reduced } = useAssemblyScroll();
  const { playing, toggle, manualSeek: seek } = useAssemblyPlayback(progress, scrollSeek);
  const [ready, setReady] = useState(false), [attempt, setAttempt] = useState(0);
  const [textOnly, setTextOnly] = useState(false);
  const selected = chapterIndex(progress), chapter = ASSEMBLY_CHAPTERS[selected];
  const onReady = useCallback(() => setReady(true), []);
  const retry = () => {
    clearRobotModelCache(); useLoader.clear(ProgressGLTFLoader, ASSEMBLY_ARM_URL); resetModelAsset(ASSEMBLY_ARM_URL);
    useLoader.clear(GLTFLoader, CUBE_MODEL.url);
    useLoader.clear(ProgressGLTFLoader, ADDITIONS_ASSET_URL); resetModelAsset(ADDITIONS_ASSET_URL);
    setReady(false); setAttempt(value => value + 1);
  };
  useEffect(() => { window.scrollTo({ top: 0, behavior: 'instant' }); }, []);
  const fallback = <div className="afr2-assembly__fallback" role="status">
    <UiIcon name="cube" size={28} /><strong>No se pudo abrir la escena 3D.</strong>
    <p>Podés reintentar o recorrer las etapas con sus explicaciones.</p>
    <Button onClick={retry}>Reintentar carga</Button>
    <Button variant="subtle" onClick={() => setTextOnly(true)}>Continuar con las etapas</Button>
  </div>;
  const percent = Math.round(progress * 100);
  const introduction = reduced ? (selected === 0 ? 1 : 0) : 1 - ease(segment(progress, .035, .065));
  const dock = reduced ? (progress > 0 ? 1 : 0) : ease(segment(progress, 0, .025));
  return <div className="afr2-route-view afr2-page-stack afr2-assembly" data-reduced={reduced}>
    <section ref={track} className="afr2-assembly__track" aria-label="Recorrido de armado controlado por scroll">
      <div ref={stage} className="afr2-assembly__stage" data-grasp={selected === ASSEMBLY_CHAPTERS.length - 1} data-additions={chapter.code === 'C600' || chapter.code === 'SENSOR A5'} data-interface={progress >= .43 && progress < .482}>
        <div className="afr2-assembly__halo" aria-hidden="true" />
        <header className="afr2-assembly__masthead">
          <p><span>Explorar</span>Armado del robot</p>
          <Link to="/modelos/conjunto">Ver modelo 3D<UiIcon name="arrow" size={16} /></Link>
        </header>
        <div className="afr2-assembly__visual" aria-label={`Presentación 3D: ${chapter.title}`}>
          {textOnly ? <div className="afr2-assembly__fallback"><UiIcon name={chapter.icon} size={40} /><strong>{chapter.short}</strong><p>Recorrido por etapas</p></div>
            : <RenderBoundary key={attempt} fallback={fallback}>
              <AssemblyScene progress={progress} reduced={reduced} onReady={onReady} fallback={fallback} />
              {!ready && <div className="afr2-assembly__loading" role="status">
                <strong>Preparando las piezas del robot…</strong>
                <ModelAssetProgress label="Auto" /><ModelAssetProgress url={ASSEMBLY_ARM_URL} label="Brazo V17" />
                <ModelAssetProgress url={ADDITIONS_ASSET_URL} label="Circuito de batería y cables" />
              </div>}
            </RenderBoundary>}
        </div>
        <div className="afr2-assembly__intro" data-docked={dock >= .5} style={{ opacity: introduction,
          '--intro-dock': dock, '--intro-scale': reduced ? 1 : 1.35 - .35 * dock,
          textAlign: dock < .5 ? 'center' : 'left',
          transform: `translate3d(${-50 * (1 - dock)}%, ${-50 * (1 - dock)}%, 0) scale(var(--intro-scale))` } as CSSProperties}>
          <p className="afr2-assembly__eyebrow">Auto Mecanum + brazo V17</p>
          <h1>De las piezas<br /><span>al movimiento.</span></h1>
          <p>Un robot recolector que toma forma con cada gesto</p>
        </div>
        <div className="afr2-assembly__captions" aria-hidden="true">
            {ASSEMBLY_CHAPTERS.map((item, index) => {
              if (index === 0) return null;
              const end = ASSEMBLY_CHAPTERS[index + 1]?.at ?? 1.1;
              const enter = index === 0 ? 1 : segment(progress, item.at, item.at + .009);
              const exit = 1 - segment(progress, end - .009, end);
              const opacity = reduced ? (index === selected ? 1 : 0) : clamp01(Math.min(enter, exit));
              const direction = item.side === 'left' ? -1 : 1;
              const style: CSSProperties = { opacity, transform: reduced ? 'none' : `translate3d(${direction * (1 - opacity) * 22}px, ${(1 - opacity) * 8}px, 0)`, filter: reduced || opacity === 0 || opacity === 1 ? 'none' : `blur(${(1 - opacity) * 3}px)` };
              return <div key={item.at} className="afr2-assembly__caption" data-side={item.side} style={style}>
                <span className="afr2-assembly__code">{item.code === 'C600' || item.code === 'SENSOR A5' ? 'Nuestros agregados' : item.short}</span><strong>{item.title}</strong>
                <span>{item.text}</span>
              </div>;
            })}
        </div>
        {progress >= .43 && progress < .482 && <AssemblyInterfaceShowcase progress={progress} reduced={reduced} />}
        <footer className="afr2-assembly__footer">
          {/* AFR2-20261110-16: at the end, the next destinations sit beside the cue (closing section removed). */}
          <div className="afr2-assembly__cue-row">
            <p className="afr2-assembly__scroll-cue"><UiIcon name="arrow-down" size={16} />{progress >= 1 ? 'Del diseño a la acción' : 'Deslizá para descubrir'}</p>
            {progress >= 1 && <div className="afr2-assembly__links afr2-assembly__links--inline">
              <Link to="/modelos/conjunto" className="afr2-infra-link">Explorar el conjunto<UiIcon name="arrow" /></Link>
              <Link to="/simulacion/conjunta" className="afr2-infra-link">Ir a simulación<UiIcon name="arrow" /></Link>
            </div>}
          </div>
          <div className="afr2-assembly__progress">
          <button type="button" className="afr2-assembly__play" data-assembly-play
            disabled={!ready && !textOnly} onClick={toggle} aria-pressed={playing}
            aria-label={playing ? 'Pausar recorrido' : `Reproducir recorrido · ${ASSEMBLY_DURATION.toFixed(1)} segundos`}
            title={playing ? 'Pausar' : `Reproducir · ${ASSEMBLY_DURATION.toFixed(1)} s`}>
            <UiIcon name={playing ? 'pause' : 'play'} size={20} />
          </button>
          <label className="sr-only" htmlFor="assembly-progress">Avance del armado del robot</label>
          <input id="assembly-progress" type="range" min={0} max={1000} value={Math.round(progress * 1000)}
            aria-valuetext={`${percent} por ciento. ${chapter.short}`} onChange={event => seek(Number(event.target.value) / 1000)} />
          <output htmlFor="assembly-progress">{String(selected + 1).padStart(2, '0')} <span>/ {ASSEMBLY_CHAPTERS.length}</span></output>
          </div>
        </footer>
        <p className="sr-only" aria-live="polite" aria-atomic="true">Etapa {selected + 1}: {chapter.title} {chapter.text}</p>
      </div>
    </section>

  </div>;
}
