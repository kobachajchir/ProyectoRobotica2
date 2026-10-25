import { UiIcon, type UiIconName } from '../../components/ui/UiIcon';

/** Decorative section illustrations. Geometry is symbolic, not a model of the chassis. */
export function SectionArtwork({ name }: { name: UiIconName }) {
  return <div className={`afr2-section-art afr2-section-art--${name}`} aria-hidden="true">
    <div className="afr2-section-art__orbit" /><div className="afr2-section-art__tile"><UiIcon name={name} /></div>
    <span className="afr2-section-art__dot" /><span className="afr2-section-art__line" />
  </div>;
}
